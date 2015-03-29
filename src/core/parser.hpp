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

    struct TrackNote
    {
        TrackNote( noteType type, float time ) : m_type(type), m_time(time) {};
        noteType m_type;
        float m_time; //msec
    };

    struct Track
    {
        Track( trackType type, diffLevel difficulty ) : m_type(type), m_diff(difficulty) {};
        trackType m_type;
        diffLevel m_diff;
        std::vector<TrackNote> notes;
        int numNotes;

        bool isSolo();
        bool isDrumRoll();
        bool isOverDrive();
        bool isBigRockEnding(); // move to Song?
        bool isTremolo();
        bool isTrill();
    };

    struct PlayerTrack
    {
        PlayerTrack( trackType type, diffLevel difficulty ) : m_type(type), m_diff(difficulty) {};
        trackType m_type;
        diffLevel m_diff;
        Track *track;
    };

    // Get the tracks for the given song, for in-game
    class Song
    {
    private:
        std::vector<Track> m_tracks;
        std::string m_path;
        int m_numPlayers;
    public:
        Song( std::string songpath ) : m_path(songpath) {};

        //TempoTrack tempoTrack;
        //EventTrack eventTrack;
        std::vector<PlayerTrack> playerTracks;

        // call addPlayer( instrument, diff ) for each player
        // store returned number, its the index in playerTracks
        // call load() to load the tracks into the indexes
        int addPlayer( trackType type, diffLevel difficulty ); // enum for diff? seperate enum for dealing with instruments vs. track types?
        bool load();
    };

    std::string TrackNameForType ( trackType type );
}