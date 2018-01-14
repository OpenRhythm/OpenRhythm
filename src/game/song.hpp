// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.
#pragma once

#include <string>
#include <vector>
#include <map>
#include <spdlog/spdlog.h>

#include "smf.hpp"
#include "timing.hpp"

#include "core/audio/vorbissource.hpp"
#include "core/audio/cubeboutput.hpp"

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
        Keys,
        Vocals,
    };

    enum class NoteType
    {
        NONE,
        Green,
        Red,
        Yellow,
        Blue,
        Orange,
        Solo,
        Drive,
        Freestyle
    };

    struct TrackNote
    {
        NoteType type;
        double time;
        int32_t tickTimeStart;
        int32_t tickTimeEnd;
        double length;
        int objNoteID;
        int objTailID;
        bool isHopo;
        bool played;
    };

    struct TempoEvent
    {
        int numerator;
        int denominator;
        int qnScaleFactor;
        int qnLength;
        double time;
        int64_t tickTime;
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
        // int count;
        double time;
    };

    enum class EventType
    {
        solo,
        drive,
        freestyle,
    };

    // Events are for things that have a position/length with no special data accociated with them
    // So drive/solo/freestyle are some examples.
    struct Event
    {
        EventType type;
        double time;
        double length;
    };


    struct TrackInfo
    {
        TrackType type;
        Difficulty difficulty;
        bool hopoSupport;
    };

    class Song;

    class TempoTrack
    {
    public:
        void set_midi(ORCore::SmfReader* midi);

        void add_tempo_event(int ppqn, double time, int64_t tickTime);
        void add_time_sig_event(int numerator, int denominator, int compoundFactor, double time, int64_t tickTime);

        std::vector<TempoEvent*> &get_events(double start, double end);
        std::vector<TempoEvent> &get_events();

        void mark_bars();
        std::vector<BarEvent*> &get_bars(double start, double end);
        std::vector<BarEvent> &get_bars();

    private:
        ORCore::SmfReader* m_midi;

    	std::vector<TempoEvent> m_tempo;
        std::vector<BarEvent> m_bars;
    };


    class Track
    {
    public:

        Track(Song* song, TrackInfo info);

        TrackInfo info();

        void add_note(NoteType type, double time, int32_t tickTime, bool on);
        std::vector<TrackNote*> &get_notes_in_frame(double start, double end);
        std::vector<TrackNote> &get_notes();

        void mark_notes();

        void set_event(EventType type, double time, bool on);
        std::vector<Event> &get_events();

    private:
        Song* m_song;
        TrackInfo m_info;
        std::vector<TrackNote> m_notes;
        std::vector<Event> m_events;
    };

    class Song
    {
    public:
        Song(std::string songpath);
        ~Song();
        void add(TrackType type, Difficulty difficulty, bool hopoSupport);
        bool load();
        void load_track(TrackInfo& trackInfo);
        void load_tracks();
        std::vector<Track> *get_tracks();
        std::vector<TrackInfo> &get_track_info();
        TempoTrack *get_tempo_track();
        int16_t get_divison();
        double length();
        void start();

        // Time methods
        double get_song_time();
        uint32_t get_song_tick_time();

        uint32_t time_to_ticks(double time);
        double ticks_to_time(uint32_t ticks);

        uint32_t time_to_ticks_length(double startTime, double length);
        uint32_t time_to_ticks_range(double startTime, double endTime);

        double ticks_to_time_length(uint32_t startTicks, uint32_t tickLength);
        double ticks_to_time_range(uint32_t startTicks, uint32_t endTicks);

        double get_audio_time();
        void set_pause(bool pause);

    private:
        ORCore::SmfReader m_midi;
        std::vector<TrackInfo> m_tracksInfo;
        std::vector<Track> m_tracks;
        TempoTrack m_tempoTrack;
        std::string m_path;
        uint32_t m_length;
        ORCore::Timer m_songTimer;
        ORCore::VorbisSource m_songOgg;
        ORCore::CubebOutput m_audioOut;
        double m_pauseTime;
        std::shared_ptr<spdlog::logger> m_logger;

    };

    // Functions are mainly used within the Song class
    const std::string diff_type_to_name(Difficulty diff);
    const TrackType get_track_type(std::string trackName);
    const std::string track_name_to_type(TrackType type);
} // namespace ORGame
