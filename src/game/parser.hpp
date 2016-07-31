#pragma once
#include <string>
#include <memory>
#include <vector>
#include <spdlog/spdlog.h>

namespace MgGame
{
    enum class GameFormat
    {
        RBN2 // Rock Band Network 2
    };

    enum class Difficulty
    {
        NONE,
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
        // Non-player track types
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

    enum class EventType
    {
        Note,
        Tempo,
        TimeSignature,
        Bar,
        Lyric,
        Other,
    };

    struct MidiNoteDefinition
    {
        int green;
        int red;
        int yellow;
        int blue;
        int orange;
        int power;
    };

    struct TrackNote
    {
        NoteType type;
        double time;
        double length;
    };

    struct TempoEvent
    {
        int qnLength;
        double time;
    };

    enum class BarType
    {
        measure,
        beat,
        upbeat, // This isn't always half of a beat one example of this is: 6/8 compound time
    };

    struct BarEvent
    {
        BarType type;
        double time;
    };

    union TempoTrackEvent
    {
        TempoEvent *tempo;
        BarEvent *bar;

        TempoTrackEvent(TempoEvent *tmpo):
        tempo(tmpo)
        {
        }

        TempoTrackEvent(BarEvent *brEvent):
        bar(brEvent)
        {
        }
    };


    class TempoTrack
    {
    private:

    	std::vector<TempoEvent> m_tempo;
        std::vector<BarEvent> m_bars;
    public:
        void add_event(int ppqn, double time);
        void mark_bars();
        std::vector<TempoTrackEvent> get_events(double start, double end, EventType type);
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
    private:
        Info m_info;
        std::vector<TrackNote> m_notes;
    };


    class Song
    {
    private:
        std::vector<Track::Info> m_trackInfo;
        std::vector<Track> m_tracks;
        TempoTrack m_tempoTrack;
        std::string m_path;
        double m_length;
        std::shared_ptr<spdlog::logger> m_logger;
    public:
        Song( std::string songpath );

        void add(TrackType type, Difficulty difficulty);
        bool load();
        Track *get_track(TrackType type, Difficulty difficulty);
        TempoTrack *getTempoTrack() { return &m_tempoTrack; };

        double length() { return m_length; };
    };

    // Functions are mainly used within the Song class
    MidiNoteDefinition get_midi_format(TrackType type, Difficulty difficulty, GameFormat gameFormat);
    NoteType midi_to_note(TrackType type, int number, Difficulty difficulty);
    TrackType get_track_type(std::string trackName);
    std::string track_name_to_type(TrackType type);
}
