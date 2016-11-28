#pragma once
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

namespace ORGame
{

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

    struct TimeSigEvent
    {
        int numerator;
        int denominator;
        int qnScaleFactor;
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
        TimeSigEvent *ts;
        BarEvent *bar;

        TempoTrackEvent(TempoEvent *tmpo):
        tempo(tmpo)
        {
        }

        TempoTrackEvent(TimeSigEvent *tsEvent):
        ts(tsEvent)
        {
        }

        TempoTrackEvent(BarEvent *brEvent):
        bar(brEvent)
        {
        }

    };


    struct TrackInfo
    {
        TrackType type;
        Difficulty difficulty;
    };

    class TempoTrack
    {
    public:
        void add_tempo_event(int ppqn, double time);
        void add_time_sig_event(int numerator, int denominator, int compoundFactor, double time);
        void mark_bars();
        std::vector<TempoTrackEvent> get_events(double start, double end, EventType type);

    private:
    	std::vector<TempoEvent> m_tempo;
        std::vector<TimeSigEvent> m_ts;
        std::vector<BarEvent> m_bars;
    };


    class Track
    {
    public:

        Track(TrackInfo info);

        TrackInfo info();

        void add_note(NoteType type, double time, bool on);
        std::vector<TrackNote*> get_notes_in_frame(double start, double end);

    private:
        TrackInfo m_info;
        std::vector<TrackNote> m_notes;
    };


    class Song
    {
    public:
        Song(std::string songpath);
        void add(TrackType type, Difficulty difficulty);
        bool load();
        Track *get_track(TrackType type, Difficulty difficulty);
        TempoTrack *get_tempo_track();

        double length();

    private:
        std::vector<TrackInfo> m_tracksInfo;
        std::vector<Track> m_tracks;
        TempoTrack m_tempoTrack;
        std::string m_path;
        double m_length;
        std::shared_ptr<spdlog::logger> m_logger;

    };

    // Functions are mainly used within the Song class
    MidiNoteDefinition get_midi_format(TrackType type, Difficulty difficulty);
    NoteType midi_to_note(TrackType type, int number, Difficulty difficulty);
    TrackType get_track_type(std::string trackName);
    std::string track_name_to_type(TrackType type);
} // namespace ORGame
