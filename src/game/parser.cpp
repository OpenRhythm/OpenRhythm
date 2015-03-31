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

    std::string TrackNameForType( trackType type )
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

    noteType noteFromEvent( trackType type, int number, diffLevel difficulty )
    {
        int min, max;

        min = noteMap[NM_ROCKBAND][difficulty][0];
        max = noteMap[NM_ROCKBAND][difficulty][1];

        if ( number < min && number > max )
            return NOTE_INVALID;

        return static_cast<noteType> (number - min + 1);
    }

    trackType TrackTypeFromString( std::string string )
    {
        if ( string.length() < 21 )
            return TRACK_NONE;
        
        if ( !string.compare(21, 4, "BEAT") ) return TRACK_BEAT;
        else if ( !string.compare(21, 5, "VENUE") ) return TRACK_VENUE;
        else if ( !string.compare(21, 6, "EVENTS") ) return TRACK_EVENTS;
        else if ( !string.compare(21, 9, "PART BASS") ) return TRACK_BASS;
        else if ( !string.compare(21, 10, "PART DRUMS" ) ) return TRACK_DRUMS;
        else if ( !string.compare(21, 11, "PART VOCALS") ) return TRACK_VOCALS;
        else if ( !string.compare(21, 11, "PART GUITAR") ) return TRACK_GUITAR;
        else return TRACK_NONE;
    }

    void Track::addNote( noteType type, float time )
    {
        m_notes.push_back( TrackNote(type, time) );
    }

    void Song::add( trackType type, diffLevel difficulty )
    {
        if ( type >= TRACK_EVENTS )
            return;

        m_trackInfo.push_back( {type, difficulty} );
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
        std::string eventBuf;
        trackType typeComp;
        noteType eTypeComp;
        int checkedTracks = 0;

        while ((sTrack = smf_get_track_by_number(smf, checkedTracks+1)) != NULL)
        {
            Track *nTrack = NULL;

            sEvent = smf_track_get_next_event(sTrack);
            eventBuf = (buf = smf_event_decode(sEvent));
            free(buf);

            typeComp = TrackTypeFromString(eventBuf);

            // Store these
            if ( typeComp == TRACK_EVENTS )
                continue;
            else if ( typeComp == TRACK_BEAT )
                continue;
            else if ( typeComp == TRACK_VENUE )
                continue;

            for (int j = 0; j < m_trackInfo.size(); j++)
            {
                if ( m_trackInfo[j].type == typeComp )
                {
                    m_tracks.push_back( Track(m_trackInfo[j]) );
                    nTrack = &m_tracks.back();
                    break;
                }
            }

            checkedTracks++;

            if ( !nTrack )
                continue;

            while ((sEvent = smf_track_get_next_event(sTrack)) != NULL)
            {
                eTypeComp = noteFromEvent(nTrack->type(), sEvent->midi_buffer[1], nTrack->difficulty());

                if ( eTypeComp != NOTE_INVALID )
                    nTrack->addNote( eTypeComp, sEvent->time_seconds / 1000 );
            }
        }

        smf_delete( smf );
        return true;
    }

    Track *Song::getTrack( trackType type, diffLevel difficulty )
    {
        for (int i = 0; i < m_tracks.size(); i++) {
            if ( m_tracks[i].type() == type && m_tracks[i].difficulty() == difficulty ) {
                return &m_tracks[i];
            }
        }

        return NULL;
    }
}