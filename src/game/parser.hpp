#pragma once
#include <string>
#include <memory>
#include <vector>

namespace MgCore
{
    enum class GameFormat
    {
        RBN2 // Rock Band Nation 2
    };

    enum class Difficulty
    {
        Easy,
        Medium,
        Hard,
        Expert
    };

    enum class TrackType
    {
        NONE,
        Guitar,
        Bass,
        Drums,
        Vocals,
        // All non-player tracks after Events
        Events,
        Venue,
        Beat
    };

    enum class NoteType
    {
        NONE,
        Green,
        Red,
        Yellow,
        Blue,
        Orange,
        //
        UpBeat,
        DownBeat
    };

    class TrackNote
    {
    private:
        NoteType m_type;
        float m_time; //msec
    public:
        TrackNote( NoteType type, float time ) : m_type(type), m_time(time) {};
        NoteType type() { return m_type; };
        float time() { return m_time; };
    };

    class Track
    {
    public:        
        struct Info
        {
            TrackType type;
            Difficulty difficulty;
        };

        Track( Info info ) : m_info(info) {};

        Info info() { return m_info; };

        void addNote ( NoteType type, float time );
        // std::vector<TrackNote*> getNotesInFrame( float start, float end );

        bool isSolo();
        bool isDrumRoll();
        bool isOverDrive();
        bool isBigRockEnding(); // move to Song?
        bool isTremolo();
        bool isTrill();
    private:
        Info m_info;
        std::vector<TrackNote> m_notes;
        int m_numNotes;
    };

    // Get the tracks for the given song, for in-game
    class Song
    {
    private:
        std::vector<Track::Info> m_trackInfo;
        std::vector<Track> m_tracks;
        std::string m_path;
        float m_length;
    public:
        Song( std::string songpath );

        void add( TrackType type, Difficulty difficulty );
        bool load();
        Track *getTrack( TrackType type, Difficulty difficulty );

        float length() { return m_length; };
    };

    std::string TrackNameForType ( TrackType type );
}