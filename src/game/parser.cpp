#include "config.hpp"
#include "parser.hpp"

#include "vfs.hpp"
#include "smf.hpp"

namespace ORGame
{


    static std::shared_ptr<spdlog::logger> logger;

    /////////////////////////////////////
    // TempoTrack Class methods
    /////////////////////////////////////

    void TempoTrack::add_tempo_event(int qnLength, double time)
    {
        m_tempo.push_back({qnLength, time});
    }

    void TempoTrack::add_time_sig_event(int numerator, int denominator, int compoundFactor, double time)
    {
        m_ts.push_back({numerator, denominator, compoundFactor, time});
    }

    void TempoTrack::mark_bars()
    {
        TempoEvent *previousTempo = nullptr;
        double lastTempo = 0;
        double beatSubdivision = 2.0; // How many times to subdivide the beat
        double beatSegments = 0.0;
        double incr = 0.0;
        for (auto &tempo : m_tempo) {
            if (previousTempo == nullptr) {
                previousTempo = &tempo;
                continue;
            }
            incr = previousTempo->qnLength / (beatSubdivision * 1000.0);
            beatSegments = static_cast<int>(((tempo.time - previousTempo->time) / incr) + 1);
            for (int i=0; i < beatSegments; i++) {
                m_bars.push_back({BarType::beat, previousTempo->time + (incr*i)});
            }
            previousTempo = &tempo;
        }
    }

    std::vector<TempoTrackEvent> TempoTrack::get_events(double start, double end, EventType type)
    {
        std::vector<TempoTrackEvent> events;
        if (type == EventType::Tempo) {
            for (auto &tempo : m_tempo) {
                if (tempo.time >= start && tempo.time <= end) {
                    events.emplace_back(&tempo);
                }
            }
        } else if (type == EventType::TimeSignature){
            for (auto &ts : m_ts) {
                if (ts.time >= start && ts.time <= end) {
                    events.emplace_back(&ts);
                }
            }
        } else if (type == EventType::Bar){
            for (auto &bar : m_bars) {
                if (bar.time >= start && bar.time <= end) {
                    events.emplace_back(&bar);
                }
            }
        }
        return events;
    }


    /////////////////////////////////////
    // Track Class
    /////////////////////////////////////

    Track::Track(TrackInfo info)
    : m_info(info)
    {
        logger = spdlog::get("default");
        // std::cout << "test" << std::endl;
    }


    TrackInfo Track::info() {
        return m_info;
    }

    void Track::add_note(NoteType type, double time, bool on)
    {
        // TODO - Fix this it looks incorrect.
        if (!on) {
            for(int i = m_notes.size(); i >= 0; i--) {
                if (m_notes[i].type == type) {
                    m_notes[i].length = time - m_notes[i].time;
                    break;
                }
            }
            return;
        }
        m_notes.push_back({type, time, 0.0});
    }

    std::vector<TrackNote*> Track::get_notes_in_frame( double start, double end )
    {
        std::vector<TrackNote*> notes;
        for ( auto &note : m_notes) {
            if (note.time >= start && note.time <= end) {
                notes.push_back( &note );
            }
        }
        return notes;
    }


    /////////////////////////////////////
    // Song Class methods
    /////////////////////////////////////

    Song::Song( std::string songpath ) : m_path(songpath)
    {
        logger = spdlog::get("default");
    }

    void Song::add( TrackType type, Difficulty difficulty )
    {
        if ( type < TrackType::Events ) {
            m_tracksInfo.push_back( {type, difficulty} );
        }
    }

    bool Song::load()
    {
        ORCore::SmfReader midi("notes.mid");

        std::vector<ORCore::SmfTrack*> midiTracks = midi.get_tracks();

        for (auto &tempo : midi.get_tempo_track()->tempo)
        {
            m_tempoTrack.add_tempo_event(tempo.qnLength, tempo.info.info.absTime);
            logger->trace(_("Tempo change recieved at time {}"), tempo.info.info.absTime);
        }

        for (auto &ts : midi.get_time_sig_track()->timeSigEvents)
        {
            m_tempoTrack.add_time_sig_event(ts.numerator, ts.denominator, ts.thirtySecondPQN/8.0, ts.info.info.absTime);
            logger->trace(_("Time signature change recieved at time {}"), ts.info.info.absTime);
        }

        m_tempoTrack.mark_bars();

        for (auto midiTrack : midiTracks)
        {
            TrackType type = get_track_type(midiTrack->name);

            for (auto &track : m_tracks)
            {
                NoteType note;
                for (auto &midiEvent : midiTrack->midiEvents)
                {
                    if (midiEvent.message == ORCore::NoteOn) {
                        note = midi_to_note(type, midiEvent.data1, track.info().difficulty);
                        track.add_note(note, midiEvent.info.absTime, true);
                    } else if (midiEvent.message == ORCore::NoteOff) {
                        note = midi_to_note(type, midiEvent.data1, track.info().difficulty);
                        track.add_note(note, midiEvent.info.absTime, false);
                    }
                }
            }
        }
        return false;
    }

    Track *Song::get_track(TrackType type, Difficulty difficulty)
    {
        for ( auto &track : m_tracks) {
            if ( track.info().type == type && track.info().difficulty == difficulty ) {
                return &track;
            }
        }

        return nullptr;
    }

    std::vector<TrackInfo> *Song::get_track_info()
    {
        return &m_tracksInfo; 
    }

    TempoTrack *Song::get_tempo_track()
    {
        return &m_tempoTrack;
    };

    double Song::length()
    {
        return m_length;
    };

    // generic functions, though they are mostly used within the Song class.

     // TODO - Basically all of these functions would be much simpler just replacing them with maps.
    MidiNoteDefinition get_midi_format(TrackType type, Difficulty difficulty)
    {
        MidiNoteDefinition notes {0, 0, 0, 0, 0, 0};
        if (type == TrackType::Guitar || type == TrackType::Bass) {
            switch(difficulty)
            {
                case Difficulty::Expert:
                {
                    notes.green = 0x60;
                    notes.red = 0x61;
                    notes.yellow = 0x62;
                    notes.blue = 0x63;
                    notes.orange = 0x64;
                    notes.power = 0x74; // should confirm this
                    break;

                }
                case Difficulty::Hard:
                {
                    notes.green = 0x54;
                    notes.red = 0x55;
                    notes.yellow = 0x56;
                    notes.blue = 0x57;
                    notes.orange = 0x58;
                    notes.power = 0x74; // should confirm this
                    break;

                }
                case Difficulty::Medium:
                {
                    notes.green = 0x48;
                    notes.red = 0x49;
                    notes.yellow = 0x4a;
                    notes.blue = 0x4b;
                    notes.orange = 0x4c;
                    notes.power = 0x74; // should confirm this
                    break;
                }
                case Difficulty::Easy:
                {
                    notes.green = 0x3c;
                    notes.red = 0x3d;
                    notes.yellow = 0x3e;
                    notes.blue = 0x3f;
                    notes.orange = 0x40;
                    notes.power = 0x74; // should confirm this
                    break;
                }
            }
        }
        return notes;
    };

    NoteType midi_to_note(TrackType type, int number, Difficulty difficulty)
    {
        MidiNoteDefinition noteFormat = get_midi_format(type, difficulty);

        if (number == noteFormat.green) {
            return NoteType::Green;
        } else if (number == noteFormat.red) {
            return NoteType::Red;
        }  else if (number == noteFormat.yellow) {
            return NoteType::Yellow;
        }  else if (number == noteFormat.blue) {
            return NoteType::Blue;
        }  else if (number == noteFormat.orange) {
            return NoteType::Orange;
        }  else if (number == noteFormat.orange) {
            return NoteType::Orange;
        }
        return NoteType::NONE;
    }

    TrackType get_track_type(std::string trackName)
    {
        if ( !trackName.compare("EVENTS") ) {
            return TrackType::Events;
        } else if ( !trackName.compare("PART BASS") ) {
            return TrackType::Bass;
        } else if ( !trackName.compare("PART DRUMS" ) ) {
            return TrackType::Drums;
        } else if ( !trackName.compare("PART VOCALS") ) {
            return TrackType::Vocals;
        } else if ( !trackName.compare("PART GUITAR") ) {
            return TrackType::Guitar;
        } else {
            return TrackType::NONE;
        }
    }

    std::string track_name_to_type(TrackType type)
    {
        switch ( type )
        {
            case TrackType::Guitar: return "Guitar";
            case TrackType::Bass: return "Bass";
            case TrackType::Drums: return "Drums";
            case TrackType::Vocals: return "Vocals";
            case TrackType::Events: return "Events";
            case TrackType::NONE:
            default: return "None/Unknown";
        }
    }

} // namespace ORGame
