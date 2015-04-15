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

    struct TempoEvent
    {
    private:
        float m_bpm;
        float m_time;
    public:
        TempoEvent(float bpm, float time) : m_bpm(bpm), m_time(time) {};
        float bpm() { return m_bpm; };
        float time() { return m_time; };
    };

    class TempoTrack
    {
    private:
        std::vector<TempoEvent> m_tempoEvents;
    public:
        void addEvent(float bpm, float time);
        std::vector<TempoEvent*> getEventsInFrame(float start, float end);
    };

    class TrackNote
    {
    private:
        NoteType m_type;
        float m_time;
    public:
        TrackNote( NoteType type, float time ) : m_type(type), m_time(time) {};
        NoteType type() { return m_type; };
        float time() { return m_time; };
        float length;
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

        void addNote(NoteType type, float time, bool on);
        std::vector<TrackNote*> getNotesInFrame(float start, float end);

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
        float m_length;
    public:
        Song( std::string songpath );

        void add(TrackType type, Difficulty difficulty);
        bool load();
        Track *getTrack(TrackType type, Difficulty difficulty);
        TempoTrack *getTempoTrack() { return &m_tempoTrack; };

        float length() { return m_length; };
    };

    std::string TrackNameForType ( TrackType type );
    std::string NoteNameForType( NoteType type );
}