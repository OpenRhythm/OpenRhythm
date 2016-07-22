#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.hpp"

namespace MgGame
{

    /////////////////////////////////////
    // TempoTrack Class methods
    /////////////////////////////////////

    void TempoTrack::addEvent(float bpm, double time)
    {
        m_tempoEvents.push_back(TempoEvent({bpm, time}));
    }

    std::vector<TempoEvent*> TempoTrack::getEventsInFrame(double start, double end)
    {
        std::vector<TempoEvent*> tempoEvents;
        for (auto &tempo : m_tempoEvents) {
            if (tempo.time >= start && tempo.time <= end) {
                tempoEvents.push_back(&tempo);
            }
        }
        return tempoEvents;
    }


    /////////////////////////////////////
    // Track Class methods
    /////////////////////////////////////

    void Track::addNote(NoteType type, double time, bool on)
    {
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

    std::vector<TrackNote*> Track::getNotesInFrame( double start, double end )
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
        // TODO - I dont really think we should be by default adding a song type
        m_trackInfo.push_back( {TrackType::Events, Difficulty::Easy} );
    }

    void Song::add( TrackType type, Difficulty difficulty )
    {
        if ( type >= TrackType::Events ) {
            return; // Non-played tracks are handled seperately
        }

        m_trackInfo.push_back( {type, difficulty} );
    }

    bool Song::load()
    {
        MgCore::SmfReader midi("notes.mid");

        auto tracks = midi.getTracks();

        for (auto track: tracks) {
            std::cout << track->name << std::endl;
        }


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
        //                 nTrack->addNote(eTypeComp, sEvent->time_seconds * 1000,  (status == 0x90) ? true : false);
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
        return NoteType::NONE
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
