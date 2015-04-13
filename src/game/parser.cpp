#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.h"

namespace MgCore
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
                if ( type == TrackType::Beat ) return {12, 13}; 
                else return {60 + (12 * static_cast<int>(difficulty)), 64 + (12 * static_cast<int>(difficulty))};
            default:
                return {0, 0};
        }
    };

    NoteType noteFromEvent( TrackType type, int number, Difficulty difficulty )
    {
        noteRange range = getNoteRange( type, difficulty, GameFormat::RBN2 );

        if ( number < range.min && number > range.max )
            return NoteType::NONE;

        if ( type < TrackType::Events )
            return static_cast<NoteType> (number - range.min + 1);
        else if ( type == TrackType::Beat )
            return (number == range.min) ? NoteType::DownBeat : NoteType::UpBeat;
        else return NoteType::NONE;
    }

    TrackType trackTypeFromString( std::string string )
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

    Song::Song( std::string songpath ) : m_path(songpath)
    {
        m_trackInfo.push_back( {TrackType::Events, Difficulty::Easy} );
        m_trackInfo.push_back( {TrackType::Venue, Difficulty::Easy} );
        m_trackInfo.push_back( {TrackType::Beat, Difficulty::Easy} );
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

            typeComp = trackTypeFromString(eventBuf);

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

    void Track::addNote( NoteType type, float time )
    {
        m_notes.push_back( TrackNote(type, time) );
    }
}