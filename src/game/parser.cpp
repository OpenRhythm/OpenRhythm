#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.h"

namespace MgCore
{
    // Move notemaps to struct
    enum noteMapTypes {
        NM_ROCKBAND,
        MAX_NOTE_MAPS
    };

    int noteMap[MAX_NOTE_MAPS][4][2] = {
        {
            {60, 64}, // Easy
            {72, 76}, // Medium
            {84, 88}, // Hard
            {96, 100} // Expert
        }
    };

    std::string TrackNameForType( TrackType type )
    {
        switch ( type )
        {
            case TrackType::Guitar: return "Guitar";
            case TrackType::Bass: return "Bass";
            case TrackType::Drums: return "Drums";
            case TrackType::Vocals: return "Vocals";
            case TrackType::Events: return "Events";
            case TrackType::Venue: return "Events";
            case TrackType::Beat: return "Beat";
            case TrackType::NONE:
            default: return "None/Unknown";
        }
    }

    NoteType noteFromEvent( TrackType type, int number, Difficulty difficulty )
    {
        int min, max;
        int diff = static_cast<int>(difficulty);

        min = noteMap[NM_ROCKBAND][diff][0];
        max = noteMap[NM_ROCKBAND][diff][1];

        if ( number < min && number > max )
            return NoteType::NONE;

        return static_cast<NoteType> (number - min + 1);
    }

    TrackType TrackTypeFromString( std::string string )
    {
        if ( string.length() < 21 )
            return TrackType::NONE;

        if ( !string.compare(21, 4, "BEAT") ) return TrackType::Beat;
        else if ( !string.compare(21, 5, "VENUE") ) return TrackType::Venue;
        else if ( !string.compare(21, 6, "EVENTS") ) return TrackType::Events;
        else if ( !string.compare(21, 9, "PART BASS") ) return TrackType::Bass;
        else if ( !string.compare(21, 10, "PART DRUMS" ) ) return TrackType::Drums;
        else if ( !string.compare(21, 11, "PART VOCALS") ) return TrackType::Vocals;
        else if ( !string.compare(21, 11, "PART GUITAR") ) return TrackType::Guitar;
        else return TrackType::NONE;
    }

    void Track::addNote( NoteType type, float time )
    {
        m_notes.push_back( TrackNote(type, time) );
    }

    Song( std::string songpath ) : m_path(songpath)
    {
        m_trackInfo.push_back( {TrackType::Events, 0} );
        m_trackInfo.push_back( {TrackType::Venue, 0} );
        m_trackInfo.push_back( {TrackType::Beat, 0} );
    }

    void Song::add( TrackType type, Difficulty difficulty )
    {
        if ( type >= TrackType::Events )
            return; // Non-played tracks are handled seperately

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
        TrackType typeComp;
        NoteType eTypeComp;
        int checkedTracks = 0;

        while ((sTrack = smf_get_track_by_number(smf, checkedTracks+1)) != NULL)
        {
            Track *nTrack = NULL;

            sEvent = smf_track_get_next_event(sTrack);
            eventBuf = (buf = smf_event_decode(sEvent));
            free(buf);

            typeComp = TrackTypeFromString(eventBuf);

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
                eTypeComp = noteFromEvent(nTrack->info().type, sEvent->midi_buffer[1], nTrack->info().difficulty);

                if ( eTypeComp != NoteType::NONE )
                    nTrack->addNote( eTypeComp, sEvent->time_seconds / 1000 );
            }
        }

        smf_delete( smf );
        return true;
    }

    Track *Song::getTrack( TrackType type, Difficulty difficulty )
    {
        for (int i = 0; i < m_tracks.size(); i++) {
            if ( m_tracks[i].info().type == type && m_tracks[i].info().difficulty == difficulty ) {
                return &m_tracks[i];
            }
        }

        return NULL;
    }
}