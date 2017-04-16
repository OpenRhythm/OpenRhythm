#pragma once
#include <string>
#include <vector>
#include <map>
#include <spdlog/spdlog.h>

#include "smf.hpp"

namespace MidiPlayer
{
    struct TrackNote
    {
        double time;
        double length;
        int noteValue;
    };

    struct TrackView
    {
        std::vector<TrackNote> *notes;
        int start;
        int end;
    };

    class Track
    {
    public:

        Track();
        void add_note(int noteValue, double time, bool on);
        TrackView get_notes_in_frame(double start, double end);
        TrackView get_notes();

    private:
        std::vector<TrackNote> m_notes;

        std::vector<int> m_noteValues;
        std::vector<double> m_time;
        std::vector<double> m_length;
    };

    class Song
    {
    public:
        Song();
        void load();
        std::vector<Track> *get_tracks();
        double length();

    private:
        ORCore::SmfReader m_midi;
        std::vector<Track> m_tracks;
        double m_length;
    };

} // namespace MidiPlayer
