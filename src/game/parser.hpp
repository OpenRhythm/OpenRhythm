#pragma once
#include <string>
#include <memory>
#include <vector>

namespace MgCore
{
    enum diffLevel {
        DIFF_EASY,
        DIFF_MEDIUM,
        DIFF_HARD,
        DIFF_EXPERT,
        MAX_DIFFICULTY
    };

    enum trackType {
        TRACK_NONE,
        TRACK_GUITAR,
        TRACK_BASS,
        TRACK_DRUMS,
        TRACK_VOCALS,
        // All non-player tracks after TRACK_EVENT
        TRACK_EVENTS,
        TRACK_VENUE,
        TRACK_BEAT
    };

    enum noteType {
        NOTE_INVALID,
        // standard notes
        NOTE_GREEN,
        NOTE_RED,
        NOTE_YELLOW,
        NOTE_BLUE,
        NOTE_ORANGE
    };

    const int TRACK_ALL = ( TRACK_GUITAR | TRACK_DRUMS | TRACK_VOCALS | TRACK_BASS );

    class TrackNote
    {
    private:
        noteType m_type;
        float m_time; //msec
    public:
        TrackNote( noteType type, float time ) : m_type(type), m_time(time) {};

        noteType type() { return m_type; };
        float time() { return m_time; };
    };

    struct trackInfo { 
        trackType type; 
        diffLevel difficulty; 
    };

    class Track
    {
    private:
        trackInfo m_info;
        std::vector<TrackNote> m_notes;
        int m_numNotes;
    public:
        Track( trackInfo info ) : m_info(info) {};
        
        trackType type() { return m_info.type; };
        diffLevel difficulty() { return m_info.difficulty; };

        void addNote ( noteType type, float time );
        // std::vector<TrackNote*> getNotesInFrame( float start, float end );

        bool isSolo();
        bool isDrumRoll();
        bool isOverDrive();
        bool isBigRockEnding(); // move to Song?
        bool isTremolo();
        bool isTrill();
    };

    // Get the tracks for the given song, for in-game
    class Song
    {
    private:
        std::vector<trackInfo> m_trackInfo;
        std::vector<Track> m_tracks;
        std::string m_path;
    public:
        Song( std::string songpath ) : m_path(songpath) {};

        void add( trackType type, diffLevel difficulty );
        bool load();
        Track *getTrack( trackType type, diffLevel difficulty );
    };

    std::string TrackNameForType ( trackType type );
}