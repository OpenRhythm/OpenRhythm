#pragma once
#include <string>
#include <memory>
#include <vector>

namespace MgGame
{
    enum class GameFormat
    {
        RBN2 // Rock Band Network 2
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
        Events
    };

    enum class NoteType
    {
        NONE,
        Green,
        Red,
        Yellow,
        Blue,
        Orange,
    };

    struct TempoEvent
    {
        float bpm;
        double time;
    };

    class TempoTrack
    {
    private:
        std::vector<TempoEvent> m_tempoEvents;
    public:
        void addEvent(float bpm, double time);
        std::vector<TempoEvent*> getEventsInFrame(double start, double end);
    };

    struct TrackNote
    {
        NoteType type;
        double time;
        double length;
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

        void addNote(NoteType type, double time, bool on);
        std::vector<TrackNote*> getNotesInFrame(double start, double end);

        bool isSolo();
        bool isDrumRoll();
        bool isOverDrive();
        bool isBigRockEnding(); // move to Song?
        bool isTremolo();
        bool isTrill();

        void listNotesInTrack();
    private:
        Info m_info;
        std::vector<TrackNote> m_notes;
    };

    // Get the tracks for the given song, for in-game
    class Song
    {
    private:
        std::vector<Track::Info> m_trackInfo;
        std::vector<Track> m_tracks;
        TempoTrack m_tempoTrack;
        std::string m_path;
        double m_length;
    public:
        Song( std::string songpath );

        void add(TrackType type, Difficulty difficulty);
        bool load();
        Track *getTrack(TrackType type, Difficulty difficulty);
        TempoTrack *getTempoTrack() { return &m_tempoTrack; };

        double length() { return m_length; };
    };

    std::string TrackNameForType ( TrackType type );
    std::string NoteNameForType( NoteType type );
}
