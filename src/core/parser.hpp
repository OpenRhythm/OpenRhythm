#pragma once
#include <string>
#include <memory>

namespace MgCore
{
    enum trackType {
        TRACK_NONE,
        TRACK_TEMPO,
        TRACK_EVENTS,
        TRACK_VENUE, // RB: ignore?
        TRACK_BEAT, // RB
        TRACK_GUITAR,
        TRACK_RHYTHM,
        TRACK_DRUMS,
        TRACK_VOCALS // Not supported
    };

    trackType TrackTypeForString ( std::string string );
    std::string TrackNameForType ( trackType type );

    struct TrackEvent
    {
        std::unique_ptr<unsigned char[]> data;
        int data_len;
    };

    struct Track
    {
        std::unique_ptr<TrackEvent[]> events;
        int numEvents;
        trackType type;
    };

    // Get the tracks for the given song, for in-game
    class TrackParser
    {
    public:
        TrackParser( std::string songpath );
        ~TrackParser();

        // make private?
        std::string m_path;
        std::unique_ptr<Track[]> m_tracks;
        int m_numTracks;
    };
}