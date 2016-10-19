#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.hpp"

#include <libintl.h>
#define _(STRING) gettext(STRING)


namespace ORGame
{

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
            incr = (previousTempo->qnLength / 1000000.0) / beatSubdivision;
            beatSegments = ((tempo.time - previousTempo->time) / incr) + 1;
            for (int i=0; i<((int)beatSegments); i++) {
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

    Track::Track(Track::Info info)
    : m_info(info)
    {
        std::cout << "test" << std::endl;
    }


    Track::Info Track::info() {
        return m_info;
    }

    void Track::add_note(NoteType type, double time, bool on)
    {
        // TODO - Fix this it looks incorrect.
        if (!on) {
            for(size_t i = m_notes.size(); i >= 0; i--) {
                if (m_notes[i].type == type) {
                    m_notes[i].length = time - m_notes[i].time;
                    break;
                }
            }
            return;
        }
        m_notes.push_back(TrackNote({type, time}));
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
        m_logger = spdlog::get("default");
    }

    void Song::add( TrackType type, Difficulty difficulty )
    {
        if ( type < TrackType::Events ) {
            m_trackInfo.push_back( {type, difficulty} );
        }
    }

    bool Song::load()
    {
        ORCore::SmfReader midi("notes.mid");

        std::vector<ORCore::SmfTrack*> tracks = midi.get_tracks();

        for (auto &tempo : midi.get_tempo_track()->tempo)
        {
            m_tempoTrack.add_tempo_event(tempo.qnLength, tempo.info.info.absTime);
            m_logger->trace(_("Tempo change recieved at time {}"), tempo.info.info.absTime);
        }

        for (auto &ts : midi.get_time_sig_track()->timeSigEvents)
        {
            m_tempoTrack.add_time_sig_event(ts.numerator, ts.denominator, ts.clocksPerBeat/24.0, ts.info.info.absTime);
            m_logger->trace(_("Time signature change recieved at time {}"), ts.info.info.absTime);
        }

        m_tempoTrack.mark_bars();


        // TODO - Finish converting to the new midi parser.
        // smf_track_t *sTrack;
        // smf_event_t *sEvent;
        // char *buf;
        // std::string eventBuf;
        // TrackType typeComp;
        // NoteType eTypeComp;
        // int checkedTracks = 1; //skip first track
        //
        // if ((sTrack = smf_get_track_by_number(smf, 1)) == NULL)
        //     return false;
        //
        // while((sEvent = smf_track_get_next_event(sTrack)) != NULL)
        // {
        //     if (!smf_event_is_metadata(sEvent) || smf_event_is_eot(sEvent))
        //         continue;
        //
        //     float bpm = 60000000.0 / (double)((sEvent->midi_buffer[3] << 16) + (sEvent->midi_buffer[4] << 8) + sEvent->midi_buffer[5]);
        //
        //     m_tempoTrack.addEvent(bpm, sEvent->time_seconds*1000);
        // }
        //
        // while ((sTrack = smf_get_track_by_number(smf, checkedTracks+1)) != NULL)
        // {
        //     Track *nTrack = NULL;
        //
        //     sEvent = smf_track_get_next_event(sTrack);
        //     eventBuf = (buf = smf_event_decode(sEvent));
        //     free(buf);
        //
        //     typeComp = get_track_type(eventBuf);
        //
        //     for (int j = 0; j < m_trackInfo.size(); j++)
        //     {
        //         if ( m_trackInfo[j].type == typeComp )
        //         {
        //             m_tracks.push_back( Track(m_trackInfo[j]) );
        //             nTrack = &m_tracks.back();
        //             break;
        //         }
        //     }
        //
        //     checkedTracks++;
        //
        //     if ( !nTrack )
        //         continue;
        //
        //     while ((sEvent = smf_track_get_next_event(sTrack)) != NULL)
        //     {
        //         if ( typeComp == TrackType::Events ) {
        //             char *tagBuf = smf_event_decode(sEvent);
        //             std::string eventTag(tagBuf);
        //             free(tagBuf);
        //
        //             if ( !eventTag.compare(6, 5, "[end]") )
        //                 m_length = sEvent->time_seconds * 1000;
        //         } else {
        //             //std::cout << smf_event_decode(sEvent) << std::endl;
        //             if (smf_event_is_metadata(sEvent) || smf_event_is_textual(sEvent))
        //                 continue;
        //
        //             int status = (sEvent->midi_buffer[0] & 0xF0);
        //
        //             if (status != 0x90 && status != 0x80) // On of Off midi notes only
        //                 continue;
        //
        //             eTypeComp = midi_to_note(nTrack->info().type, sEvent->midi_buffer[1], nTrack->info().difficulty);
        //
        //             if ( eTypeComp != NoteType::NONE )
        //                 nTrack->add_note(eTypeComp, sEvent->time_seconds * 1000,  (status == 0x90) ? true : false);
        //
        //         }
        //     }
        // }
        //
        // smf_delete( smf );
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

    TempoTrack *Song::get_tempo_track() {
        return &m_tempoTrack;
    };

    double Song::length() {
        return m_length;
    };

    // generic functions, though they are mostly used within the Song class.

    /*
     *  TODO - This will only currently properly handle a single instrument of
     *  classical guitar/drums gameplay. It needs to eventually be able to
     *  handle stuff like vocals, pro instruments, etc.
     */
    MidiNoteDefinition get_midi_format(TrackType type, Difficulty difficulty, GameFormat gameFormat)
    {
        MidiNoteDefinition notes;
        if (gameFormat == GameFormat::RBN2)
        {
            switch(difficulty)
            {
                case Difficulty::Expert: {
                    notes.green = 0x60;
                    notes.red = 0x61;
                    notes.yellow = 0x62;
                    notes.blue = 0x63;
                    notes.orange = 0x64;
                    notes.power = 0x74; // should confirm this
                    break;

                }
                case Difficulty::Hard: {
                    notes.green = 0x54;
                    notes.red = 0x55;
                    notes.yellow = 0x56;
                    notes.blue = 0x57;
                    notes.orange = 0x58;
                    notes.power = 0x74; // should confirm this
                    break;

                }
                case Difficulty::Medium: {
                    notes.green = 0x48;
                    notes.red = 0x49;
                    notes.yellow = 0x4a;
                    notes.blue = 0x4b;
                    notes.orange = 0x4c;
                    notes.power = 0x74; // should confirm this
                    break;
                }
                case Difficulty::Easy: {
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
        MidiNoteDefinition noteFormat = get_midi_format(type, difficulty, GameFormat::RBN2);

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

}
