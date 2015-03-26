#include <iostream>

#include "parser.hpp"
#include "vfs.hpp"
#include "smf.h"

namespace MgCore
{
    trackType TrackTypeForString ( std::string string )
    {
        if ( !string.compare(0, 5, "Tempo") ) return TRACK_TEMPO;
        else
        {
            if ( !string.compare(21, 6, "EVENTS") ) return TRACK_EVENTS;
            else if ( !string.compare(21, 11, "PART GUITAR") ) return TRACK_GUITAR;
            else if ( !string.compare(21, 9, "PART BASS") ) return TRACK_RHYTHM;
            else if ( !string.compare(21, 10, "PART DRUMS" ) ) return TRACK_DRUMS;
            else if ( !string.compare(21, 11, "PART VOCALS") ) return TRACK_VOCALS;
            else return TRACK_NONE;
        }
    }

    std::string TrackNameForType ( trackType type )
    {
        switch ( type )
        {
            case TRACK_TEMPO: return "Tempo";
            case TRACK_EVENTS: return "Events";
            case TRACK_GUITAR: return "Guitar";
            case TRACK_RHYTHM: return "Bass";
            case TRACK_DRUMS: return "Drums";
            case TRACK_VOCALS: return "Vocals";
            case TRACK_NONE:
            default: return "None/Unknown";
        }
    }

    std::unique_ptr<TrackEvent> Track::getTrackEventsInFrame( float start, float end )
    {
        // Find any notes that should be displayed
        return nullptr;
    }

    TrackParser::TrackParser( std::string songpath )
    : m_path (songpath)
    {
        std::string mem_buf = MgCore::read_file(m_path + "/notes.mid", FileMode::Binary);
        smf_t *smf = smf_load_from_memory( mem_buf.c_str(), mem_buf.size() );

        if ( smf == NULL )
            return;

        m_numTracks = smf->number_of_tracks;
        m_tracks = std::make_unique<Track[]>( m_numTracks );

        smf_track_t *sTrack;
        smf_event_t *sEvent;
        char *buf;
        Track *nTrack;
        TrackEvent *nEvent;
        std::string eventBuf;

        for (int i = 0; i < m_numTracks; i++)
        {
            sTrack = smf_get_track_by_number(smf, i+1);
            sEvent = smf_track_get_next_event(sTrack);
            eventBuf = (buf = smf_event_decode(sEvent));

            nTrack = &m_tracks[i];
            
            nTrack->type = TrackTypeForString(eventBuf);
            
            nTrack->numEvents = sTrack->number_of_events;
            nTrack->events = std::make_unique<TrackEvent[]>( nTrack->numEvents );
            free(buf);

            for (int j = 0; j < nTrack->numEvents; j++)
            {
                nEvent = &nTrack->events[j];

                nEvent->data_len = sEvent->midi_buffer_length;
                nEvent->data = std::make_unique<unsigned char[]>( nEvent->data_len );

                for (int k = 0; k < nEvent->data_len; k++)
                    nEvent->data[k] = sEvent->midi_buffer[k];

                if ( (sEvent = smf_track_get_next_event(sTrack)) != NULL )
                    break;
            }
        }
    }

    TrackParser::~TrackParser()
    {
        // needed?
    }
}