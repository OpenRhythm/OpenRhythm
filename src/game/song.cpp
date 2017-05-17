#include "config.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "song.hpp"

#include "vfs.hpp"

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
            incr = previousTempo->qnLength / (beatSubdivision*1'000'000.0);
            beatSegments = static_cast<int>((tempo.time - previousTempo->time) / incr)+1;
            for (int i=0; i < beatSegments; i++) {
                m_bars.push_back({BarType::beat, previousTempo->time + (incr*i)});
            }
            previousTempo = &tempo;
        }
    }

    std::vector<TempoTrackEvent> TempoTrack::get_events(double start, double end, TempoEventType type)
    {
        std::vector<TempoTrackEvent> events;
        if (type == TempoEventType::Tempo) {
            for (auto &tempo : m_tempo) {
                if (tempo.time >= start && tempo.time <= end) {
                    events.emplace_back(&tempo);
                }
            }
        } else if (type == TempoEventType::TimeSignature){
            for (auto &ts : m_ts) {
                if (ts.time >= start && ts.time <= end) {
                    events.emplace_back(&ts);
                }
            }
        } else if (type == TempoEventType::Bar){
            for (auto &bar : m_bars) {
                if (bar.time >= start && bar.time <= end) {
                    events.emplace_back(&bar);
                }
            }
        }
        return events;
    }

    std::vector<TempoTrackEvent> TempoTrack::get_events(TempoEventType type)
    {
        std::vector<TempoTrackEvent> events;
        if (type == TempoEventType::Tempo) {
            for (auto &tempo : m_tempo) {
                events.emplace_back(&tempo);
            }
        } else if (type == TempoEventType::TimeSignature){
            for (auto &ts : m_ts) {
                events.emplace_back(&ts);
            }
        } else if (type == TempoEventType::Bar){
            for (auto &bar : m_bars) {
                events.emplace_back(&bar);
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
    }


    TrackInfo Track::info() {
        return m_info;
    }

    void Track::add_note(NoteType type, double time, bool on)
    {
        static std::vector<std::pair<NoteType, int>> activeNotes;

        if (on) {
            int index = m_notes.size();
            activeNotes.emplace_back(type, index);
            m_notes.push_back({type, time, 0.0});
        } else {
            auto findFunc = [&](const auto& element)
            {
                return element.first == type;
            };
            auto item = std::find_if( activeNotes.begin(), activeNotes.end(), findFunc);
            if (item != activeNotes.end())
            {
                auto &note = m_notes[item->second];
                note.length = time - note.time;
                activeNotes.erase(item);
            }
        }
    }

    void Track::set_event(EventType type, double time, bool on)
    {
        if (on) {
            m_events.push_back({type, time, 0.0});
        } else {
            auto &solo = m_events.back();
            solo.length = time - solo.time;
        }
    }

    std::vector<Event> *Track::get_events()
    {
        return &m_events;
    }

    // void set_

    std::vector<TrackNote*> Track::get_notes_in_frame(double start, double end)
    {   
        std::vector<TrackNote*> notes;
        for ( auto &note : m_notes) {
            if (note.time >= start && note.time <= end) {
                notes.emplace_back( &note );
            }
        }
        return notes;
    }

    std::vector<TrackNote*> Track::get_notes()
    {   
        std::vector<TrackNote*> notes;
        for ( auto &note : m_notes) {
            notes.emplace_back( &note );
        }
        return notes;
    }

    using MidiNoteMap = std::map<int, NoteType>;
    const int solo_marker = 0x67;
    const int drive_marker = 0x74;
    const std::map<Difficulty, MidiNoteMap> midiDiffMap {
        {Difficulty::Expert, {
                {0x60, NoteType::Green},
                {0x61, NoteType::Red},
                {0x62, NoteType::Yellow},
                {0x63, NoteType::Blue},
                {0x64, NoteType::Orange}
            }
        },
        {Difficulty::Hard, {
                {0x54, NoteType::Green},
                {0x55, NoteType::Red},
                {0x56, NoteType::Yellow},
                {0x57, NoteType::Blue},
                {0x58, NoteType::Orange}
            }
        },
        {Difficulty::Medium, {
                {0x48, NoteType::Green},
                {0x49, NoteType::Red},
                {0x4a, NoteType::Yellow},
                {0x4b, NoteType::Blue},
                {0x4c, NoteType::Orange}
            }
        },
        {Difficulty::Easy, {
                {0x3c, NoteType::Green},
                {0x3d, NoteType::Red},
                {0x3e, NoteType::Yellow},
                {0x3f, NoteType::Blue},
                {0x40, NoteType::Orange}
            }
        }
    };

    const std::map<Difficulty, std::string> diffNameMap {
        {Difficulty::Expert, "Expert"},
        {Difficulty::Hard, "Hard"},
        {Difficulty::Medium, "Medium"},
        {Difficulty::Easy, "Easy"}
    };

    const std::map<TrackType, std::string> trackNameMap {
        {TrackType::Guitar, "Guitar"},
        {TrackType::Bass, "Bass"},
        {TrackType::Drums, "Drums"},
        {TrackType::Vocals, "Vocals"},
        {TrackType::Events, "Events"},
        {TrackType::NONE, "None"}
    };

    const std::map<std::string, TrackType> midiTrackTypeMap {
        {"PART GUITAR", TrackType::Guitar},
        {"PART BASS", TrackType::Bass},
        {"PART DRUMS", TrackType::Drums},
        {"PART VOCALS", TrackType::Vocals},
        {"EVENTS", TrackType::Events},
        {"", TrackType::NONE}
    };

    // Convenence functions for accessing the maps.
    const std::string diff_type_to_name(Difficulty diff)
    {
        try {
            return diffNameMap.at(diff);
        } catch (std::out_of_range &err) {
            return "";
        }
    }

    const std::string track_type_to_name(TrackType type)
    {
        try {
            return trackNameMap.at(type);
        } catch (std::out_of_range &err) {
            return "";
        }
    }

    const TrackType get_track_type(std::string trackName)
    {
        try {
            return midiTrackTypeMap.at(trackName);
        } catch (std::out_of_range &err) {
            return TrackType::NONE;
        }
    }

    /////////////////////////////////////
    // Song Class methods
    /////////////////////////////////////

    Song::Song(std::string songpath)
    : m_path(songpath),
    m_midi("notes.mid")
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

        for (auto &tempo : m_midi.get_tempo_track()->tempo)
        {
            m_tempoTrack.add_tempo_event(tempo.qnLength, tempo.absTime);
            logger->trace(_("Tempo change recieved at time {}"), tempo.absTime);
        }

        // for (auto &ts : m_midi.get_time_sig_track()->timeSigEvents)
        // {
        //     m_tempoTrack.add_time_sig_event(ts.numerator, ts.denominator, ts.thirtySecondPQN/8.0, ts.info.info.absTime);
        //     logger->trace(_("Time signature change recieved at time {}"), ts.info.info.absTime);
        // }

        m_tempoTrack.mark_bars();

        std::vector<ORCore::SmfTrack*> midiTracks = m_midi.get_tracks();

        bool foundUsable = false;
        
        for (auto midiTrack : midiTracks)
        {
            TrackType type = get_track_type(midiTrack->name);
            if (type == TrackType::Guitar)
            {
                // Add all difficulties for this track
                add(type, Difficulty::Expert);
                add(type, Difficulty::Hard);
                add(type, Difficulty::Medium);
                add(type, Difficulty::Easy);
                foundUsable = true;
            }
        }
        if (!foundUsable)
        {
            throw std::runtime_error("Invalid song format.");
        }

        logger->debug(_("Song loaded"));

        return false;
    }

    void Song::load_track(TrackInfo& trackInfo)
    {
        Track track(trackInfo);

        logger->debug(_("Loading Track {} {}"), track_type_to_name(trackInfo.type), diff_type_to_name(trackInfo.difficulty));

        std::vector<ORCore::SmfTrack*> midiTracks = m_midi.get_tracks();

        const MidiNoteMap &noteMap = midiDiffMap.at(trackInfo.difficulty);

        NoteType note;

        for (auto midiTrack : midiTracks)
        {
            TrackType type = get_track_type(midiTrack->name);
            if (type == trackInfo.type)
            {

                for (auto &midiEvent : midiTrack->midiEvents)
                {
                    // TODO - Clean this up this WILL get very messy eventually.
                    if (midiEvent.message == ORCore::NoteOn) {
                        if (midiEvent.data1 == solo_marker) {
                            track.set_event(EventType::solo, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), true);
                        } else if (midiEvent.data1 == drive_marker) {
                            track.set_event(EventType::drive, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), true);
                        } else {
                            try {
                                note = noteMap.at(midiEvent.data1);
                            } catch (std::out_of_range &err) {
                                continue;
                            }
                            // TODO - read note velocity and treat vel 0 as note off.
                            if (note != NoteType::NONE) {
                                track.add_note(note, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), true);
                            }
                        }
                    } else if (midiEvent.message == ORCore::NoteOff) {
                        if (midiEvent.data1 == solo_marker) {
                            track.set_event(EventType::solo, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), false);
                        } else if (midiEvent.data1 == drive_marker) {
                            track.set_event(EventType::drive, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), false);
                        } else {
                            try {
                                note = noteMap.at(midiEvent.data1);
                            } catch (std::out_of_range &err) {
                                continue;
                            }
                            if (note != NoteType::NONE) {
                                track.add_note(note, m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime), false);
                            }
                        }
                    }

                }
                m_length = midiTrack->endTime;
                break;
            }
        }
        m_tracks.push_back(track);
    }

    // Load all tracks
    void Song::load_tracks()
    {
        for (auto &trackInfo : m_tracksInfo)
        {
            load_track(trackInfo);
        }
        logger->debug(_("{} Tracks processed"), m_tracks.size());
    }

    std::vector<Track> *Song::get_tracks()
    {
        return &m_tracks;
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

} // namespace ORGame
