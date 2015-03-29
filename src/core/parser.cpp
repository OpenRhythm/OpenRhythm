#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.h"

namespace MgCore
{
    enum noteMapTypes {
        NM_ROCKBAND,
        MAX_NOTE_MAPS
    };

    int noteMap[MAX_NOTE_MAPS][MAX_DIFFICULTY][2] = {
        {
            {60, 64}, // Easy
            {72, 76}, // Medium
            {84, 88}, // Hard
            {96, 100} // Expert
        }
    };

    trackType TrackTypeForString ( std::string string )
    {
        if ( !string.compare(21, 4, "BEAT") ) return TRACK_BEAT;
        else if ( !string.compare(21, 5, "VENUE") ) return TRACK_VENUE;
        else if ( !string.compare(21, 6, "EVENTS") ) return TRACK_EVENTS;
        else if ( !string.compare(21, 9, "PART BASS") ) return TRACK_BASS;
        else if ( !string.compare(21, 10, "PART DRUMS" ) ) return TRACK_DRUMS;
        else if ( !string.compare(21, 11, "PART VOCALS") ) return TRACK_VOCALS;
        else if ( !string.compare(21, 11, "PART GUITAR") ) return TRACK_GUITAR;
        else return TRACK_NONE;
    }

    std::string TrackNameForType ( trackType type )
    {
        switch ( type )
        {
            case TRACK_BEAT: return "Beat";
            case TRACK_EVENTS: return "Events";
            case TRACK_GUITAR: return "Guitar";
            case TRACK_BASS: return "Bass";
            case TRACK_DRUMS: return "Drums";
            case TRACK_VOCALS: return "Vocals";
            case TRACK_NONE:
            default: return "None/Unknown";
        }
    }

    noteType noteFromEvent ( trackType type, int number, diffLevel difficulty )
    {
        int min, max;

        min = noteMap[NM_ROCKBAND][difficulty][0];
        max = noteMap[NM_ROCKBAND][difficulty][1];

        if ( number < min && number > max )
            return NOTE_INVALID;

        return static_cast<noteType> (number - min + 1);
    }

    int Song::addPlayer( trackType type, diffLevel difficulty )
    {
        if ( type >= TRACK_EVENTS )
            return 0;

        playerTracks.push_back( PlayerTrack( type, difficulty ) );
        return playerTracks.size()-1;
    }

    bool Song::load()
    {
        std::string mem_buf = MgCore::read_file(m_path + "/notes.mid", FileMode::Binary);
        smf_t *smf = smf_load_from_memory( mem_buf.c_str(), mem_buf.size() );

        if ( smf == NULL )
            return false;

        smf_track_t *sTrack;
        smf_event_t *sEvent;
        char *buf;
        Track *nTrack;
        std::string eventBuf;
        trackType typeComp;
        noteType eTypeComp;
        int checkedTracks = 0;

        while ((sTrack = smf_get_track_by_number(smf, checkedTracks+1)) != NULL)
        {
            sEvent = smf_track_get_next_event(sTrack);
            eventBuf = (buf = smf_event_decode(sEvent));
            free(buf);

            typeComp = TrackTypeForString(eventBuf);

            nTrack = NULL;

            for (int j = 0; j < playerTracks.size(); j++) 
            {
                if ( playerTracks[j].m_type == typeComp ) {
                    m_tracks.push_back( Track(playerTracks[j].m_type, playerTracks[j].m_diff) );
                    playerTracks[j].track = &m_tracks.back();
                    break;
                }
            }

            checkedTracks++;

            if ( nTrack == NULL )
                continue;

            while ((sEvent = smf_track_get_next_event(sTrack)) != NULL)
            {
                eTypeComp = noteFromEvent(nTrack->m_type, sEvent->midi_buffer[1], nTrack->m_diff);

                if ( eTypeComp != NOTE_INVALID )
                    nTrack->notes.push_back( TrackNote(eTypeComp, sEvent->time_seconds / 1000) );
            }
        }

        return true;
    }
}