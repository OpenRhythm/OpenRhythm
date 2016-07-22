#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.hpp"

namespace MgGame
{
    struct noteRange
    {
        int min;
        int max;
    };

    noteRange getNoteRange( TrackType type, Difficulty difficulty, GameFormat gameFormat )
    {
        // this is kinda ugly but should work
        switch ( gameFormat )
        {
            case GameFormat::RBN2:
                return {60 + (12 * static_cast<int>(difficulty)), 64 + (12 * static_cast<int>(difficulty))};
            default:
                return {0, 0};
        }
    };

    NoteType noteFromEvent( TrackType type, int number, Difficulty difficulty )
    {
        noteRange range = getNoteRange( type, difficulty, GameFormat::RBN2 );

        if ( number < range.min || number > range.max ) {
            return NoteType::NONE;
        }

        if ( type < TrackType::Events ) {
            return static_cast<NoteType> (number - range.min + 1);
        } else {
            return NoteType::NONE;
        }
    }

    TrackType trackTypeFromString( std::string string )
    {
        if ( string.length() < 21 ) {
            return TrackType::NONE;
        }

        if ( !string.compare(21, 6, "EVENTS") ) {
            return TrackType::Events;
        } else if ( !string.compare(21, 9, "PART BASS") ) {
            return TrackType::Bass;
        } else if ( !string.compare(21, 10, "PART DRUMS" ) ) {
            return TrackType::Drums;
        } else if ( !string.compare(21, 11, "PART VOCALS") ) {
            return TrackType::Vocals;
        } else if ( !string.compare(21, 11, "PART GUITAR") ) {
            return TrackType::Guitar;
        } else {
            return TrackType::NONE;
        }
    }

    std::string TrackNameForType( TrackType type )
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

    std::string NoteNameForType( NoteType type )
    {
        switch ( type )
        {
            case NoteType::Green: return "Green";
            case NoteType::Red: return "Red";
            case NoteType::Yellow: return "Yellow";
            case NoteType::Blue: return "Blue";
            case NoteType::Orange: return "Orange";
            case NoteType::NONE:
            default: return "None";
        }
    }

    Song::Song( std::string songpath ) : m_path(songpath)
    {
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
        //     typeComp = trackTypeFromString(eventBuf);
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
        //             eTypeComp = noteFromEvent(nTrack->info().type, sEvent->midi_buffer[1], nTrack->info().difficulty);
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

    Track *Song::getTrack( TrackType type, Difficulty difficulty )
    {
        for (size_t i = 0; i < m_tracks.size(); i++) {
            if ( m_tracks[i].info().type == type && m_tracks[i].info().difficulty == difficulty ) {
                return &m_tracks[i];
            }
        }

        return nullptr;
    }

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

    void Track::listNotesInTrack()
    {
        for(size_t i = 0; i < m_notes.size(); i++) {
            std::cout << NoteNameForType(m_notes[i].type)
                      << " note at " << m_notes[i].time/1000
                      << " seconds (length " << m_notes[i].length << "ms)"
                      << std::endl;
        }

        std::cout << m_notes.size() << " notes." << std::endl;
    }

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
}
