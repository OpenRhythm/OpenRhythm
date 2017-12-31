// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "song.hpp"

#include "filesystem.hpp"

namespace ORGame
{
    static std::shared_ptr<spdlog::logger> logger;

    /////////////////////////////////////
    // TempoTrack Class methods
    /////////////////////////////////////


    void TempoTrack::set_midi(ORCore::SmfReader* midi)
    {
        m_midi = midi;
    }

    void TempoTrack::add_tempo_event(int qnLength, double time, int64_t tickTime)
    {
        // If this is the first change in tempo/timesignature create an event with the other half of state
        if (m_tempo.size() == 0)
        {
            m_tempo.push_back({0, 0, 0, qnLength, time, tickTime});
        }
        else
        {
            // If the previous event is located at the same time as the current one overwrite the data to reflect the current state.
            auto &eventLast = m_tempo.back();
            if (eventLast.time == time || eventLast.tickTime == tickTime)
            {
                eventLast.qnLength = qnLength;
            }
            else
            {
                // Otherwise we copy the previous change and update the data to reflect the new state.
                TempoEvent tempoEvent = eventLast;
                tempoEvent.qnLength = qnLength;
                tempoEvent.time = time;
                tempoEvent.tickTime = tickTime;
                m_tempo.push_back(tempoEvent);

            }
        }
    }

    void TempoTrack::add_time_sig_event(int numerator, int denominator, int compoundFactor, double time, int64_t tickTime)
    {
        // If this is the first change in tempo/timesignature create an event with the other half of state
        if (m_tempo.size() == 0)
        {
            m_tempo.push_back({numerator, denominator, compoundFactor, 0, time, tickTime});
        }
        else
        {
            // If the previous event is located at the same time as the current one overwrite the data to reflect the current state.
            auto &eventLast = m_tempo.back();
            if (eventLast.time == time)
            {
                eventLast.numerator = numerator;
                eventLast.denominator = denominator;
                eventLast.qnScaleFactor = compoundFactor;
            }
            else
            {
                // Otherwise we copy the previous change and update the data to reflect the new state.
                TempoEvent tempoEvent = eventLast;
                tempoEvent.time = time;
                tempoEvent.numerator = numerator;
                tempoEvent.denominator = denominator;
                tempoEvent.qnScaleFactor = compoundFactor;

                m_tempo.push_back(tempoEvent);
            }
        }
    }

    std::vector<TempoEvent*> &TempoTrack::get_events(double start, double end)
    {
        static std::vector<TempoEvent*> events;
        events.clear();
        for (auto &tempo : m_tempo)
        {
            if (tempo.time >= start && tempo.time <= end)
            {
                events.push_back(&tempo);
            }
        }
        return events;
    }

    std::vector<TempoEvent> &TempoTrack::get_events()
    {
        return m_tempo;
    }

    void TempoTrack::mark_bars()
    {
        TempoEvent *currentTempo = nullptr;

        int beatSubdivision = 1; // How many times to subdivide the beat
        int remainingTicks = 0;

        for (auto &nextTempo : m_tempo)
        {

            if (currentTempo == nullptr)
            {
                currentTempo = &nextTempo;
                continue;
            }

            if (currentTempo->denominator == 2)
            {
                beatSubdivision = 2;
            }
            else if (currentTempo->denominator == 4)
            {
                beatSubdivision = 2;
            }
            else
            {
                beatSubdivision = 1;
            }

            uint32_t beatTicks = (m_midi->get_header()->division * 4) / currentTempo->denominator;

            uint32_t incr = beatTicks / beatSubdivision;

            uint32_t tickPeriodBegin = currentTempo->tickTime + remainingTicks;

            uint32_t tickTimePeriod = nextTempo.tickTime - tickPeriodBegin;

            remainingTicks = 0;

            // We cant mark beats for tempo changes that are smaller than a beat.
            // accumulate and continue to next tempo event.
            if (tickTimePeriod < incr)
            {
                remainingTicks += tickTimePeriod;
                currentTempo = &nextTempo;
                continue;
            }

            uint32_t beats = tickTimePeriod / incr;

            uint32_t unusedTicks = tickTimePeriod - (beats * incr);

            // add any leftover ticks to the accumulator
            if (unusedTicks != 0)
            {
                remainingTicks += incr - unusedTicks;
            }

            int interMeasureBeatCount = 0;

            for (int i=0; i < beats; i++)
            {
                uint32_t beatTickPos = tickPeriodBegin + (incr*i);

                if (interMeasureBeatCount == 0)
                {
                    m_bars.push_back({BarType::measure, m_midi->pulsetime_to_abstime(beatTickPos)});
                }
                else if (interMeasureBeatCount % beatSubdivision == 1)
                {
                    m_bars.push_back({BarType::upbeat, m_midi->pulsetime_to_abstime(beatTickPos)});
                }
                else
                {
                    m_bars.push_back({BarType::beat, m_midi->pulsetime_to_abstime(beatTickPos)});
                }

                interMeasureBeatCount++;

                if (interMeasureBeatCount >= (currentTempo->numerator * beatSubdivision))
                {
                    interMeasureBeatCount = 0;
                }
            }

            if (currentTempo->numerator != nextTempo.numerator || currentTempo->denominator != nextTempo.denominator)
            {
                interMeasureBeatCount = 0;
            }

            currentTempo = &nextTempo;
        }
    }

    std::vector<BarEvent*> &TempoTrack::get_bars(double start, double end)
    {
        static std::vector<BarEvent*> events;
        events.clear();
        for (auto &bar : m_bars)
        {
            if (bar.time >= start && bar.time <= end)
            {
                events.push_back(&bar);
            }
        }
        return events;
    }

    std::vector<BarEvent> &TempoTrack::get_bars()
    {
        return m_bars;
    }

    /////////////////////////////////////
    // Track Class
    /////////////////////////////////////

    Track::Track(Song* song, TrackInfo info)
    : m_song(song), m_info(info)
    {
        logger = spdlog::get("default");
    }


    TrackInfo Track::info() {
        return m_info;
    }

    void Track::add_note(NoteType type, double time, int32_t pulseTime, bool on)
    {
        static std::vector<std::pair<NoteType, int>> activeNotes;

        if (on)
        {
            int index = m_notes.size();
            activeNotes.emplace_back(type, index);
            m_notes.push_back({type, time, pulseTime});
        }
        else
        {
            auto findFunc = [&](const auto& element)
            {
                return element.first == type;
            };
            auto item = std::find_if(activeNotes.begin(), activeNotes.end(), findFunc);
            if (item != activeNotes.end())
            {

                auto &note = m_notes[item->second];

                int tailCutoff = std::ceil(m_song->get_divison()/3.0);
                int pulseLength = pulseTime - note.pulseTimeStart;


                if (pulseLength <= tailCutoff)
                {
                    note.length = 0.0;
                    note.pulseTimeEnd = note.pulseTimeStart;
                }
                else
                {
                    note.length = time - note.time;
                    note.pulseTimeEnd = pulseTime;
                }
                activeNotes.erase(item);
            }
        }
    }

    void Track::set_event(EventType type, double time, bool on)
    {
        static std::vector<std::pair<EventType, int>> activeEvents;

        if (on)
        {
            int index = m_events.size();
            activeEvents.emplace_back(type, index);
            m_events.push_back({type, time, 0.0});
        }
        else
        {
            auto findFunc = [&](const auto& element)
            {
                return element.first == type;
            };
            auto item = std::find_if(activeEvents.begin(), activeEvents.end(), findFunc);
            if (item != activeEvents.end())
            {
                auto &event = m_events[item->second];
                event.length = time - event.time;
                activeEvents.erase(item);
            }
        }
    }

    std::vector<Event> &Track::get_events()
    {
        return m_events;
    }

    std::vector<TrackNote*> &Track::get_notes_in_frame(double start, double end)
    {   
        static std::vector<TrackNote*> notes;
        // The idea here is to reuse the same vector each frame to reduce allocations.
        notes.clear();
        for ( auto &note : m_notes)
        {
            if (note.time >= start && note.time <= end)
            {
                notes.emplace_back(&note);
            }
        }
        return notes;
    }

    std::vector<TrackNote> &Track::get_notes()
    {
        return m_notes;
    }

    /////////////////////////////////////
    // Conversion functions for taking data from the midi into internal formats.
    /////////////////////////////////////

    // Convert midi notes into a note type
    NoteType midi_to_note_type(Difficulty diff, int midiNote)
    {
        // Difficulty based notes
        switch(diff)
        {
            case Difficulty::Expert:
                switch(midiNote)
                {
                    case 0x60: return NoteType::Green;
                    case 0x61: return NoteType::Red;
                    case 0x62: return NoteType::Yellow;
                    case 0x63: return NoteType::Blue;
                    case 0x64: return NoteType::Orange;
                    default: break;
                }
                break;

            case Difficulty::Hard:
                switch(midiNote)
                {
                    case 0x54: return NoteType::Green;
                    case 0x55: return NoteType::Red;
                    case 0x56: return NoteType::Yellow;
                    case 0x57: return NoteType::Blue;
                    case 0x58: return NoteType::Orange;
                    default: break;
                }
                break;

            case Difficulty::Medium:
                switch(midiNote)
                {
                    case 0x48: return NoteType::Green;
                    case 0x49: return NoteType::Red;
                    case 0x4a: return NoteType::Yellow;
                    case 0x4b: return NoteType::Blue;
                    case 0x4c: return NoteType::Orange;
                    default: break;
                }
                break;

            case Difficulty::Easy:
                switch(midiNote)
                {
                    case 0x3c: return NoteType::Green;
                    case 0x3d: return NoteType::Red;
                    case 0x3e: return NoteType::Yellow;
                    case 0x3f: return NoteType::Blue;
                    case 0x40: return NoteType::Orange;
                    default: break;
                }
                break;

            default: break;
        }
        
        // Global note types
        switch(midiNote)
        {
            case 0x67: return NoteType::Solo;
            case 0x74: return NoteType::Drive;
            default: return NoteType::NONE;
        }
    }

    const std::string diff_type_to_name(Difficulty diff)
    {
        switch(diff)
        {
            case Difficulty::Expert: return "Expert";
            case Difficulty::Hard: return "Hard";
            case Difficulty::Medium: return "Medium";
            case Difficulty::Easy: return "Easy";
            default: return "";
        }
    }

    const std::string track_type_to_name(TrackType type)
    {
        switch(type)
        {
            case TrackType::Guitar: return "Guitar";
            case TrackType::Bass: return "Bass";
            case TrackType::Drums: return "Drums";
            case TrackType::Keys: return "Keys";
            case TrackType::Vocals: return "Vocals";
            default: return "None";
        }
    }

    static const std::map<std::string, TrackType> midiTrackTypeMap {
        {"PART GUITAR", TrackType::Guitar},
        {"PART BASS", TrackType::Bass},
        {"PART DRUMS", TrackType::Drums},
        {"PART KEYS", TrackType::Keys},
        {"PART VOCALS", TrackType::Vocals},
        {"", TrackType::NONE}
    };

    const TrackType get_track_type(std::string trackName)
    {
        try
        {
            return midiTrackTypeMap.at(trackName);
        }
        catch (std::out_of_range &err)
        {
            err;
            return TrackType::NONE;
        }
    }

    /////////////////////////////////////
    // Song Class methods
    /////////////////////////////////////

    Song::Song(std::string songpath)
    : m_path(songpath),
    m_midi("notes.mid"),
    m_songOgg("song.ogg"),
    m_logger(spdlog::get("default"))
    {
        logger = spdlog::get("default");

        m_audioOut.set_source(&m_songOgg);
        m_tempoTrack.set_midi(&m_midi);
    }

    Song::~Song()
    {
        m_audioOut.stop();
    }

    void Song::add(TrackType type, Difficulty difficulty)
    {
        if (type != TrackType::NONE)
        {
            m_tracksInfo.push_back({type, difficulty});
        }
    }

    bool Song::load()
    {

        std::vector<ORCore::SmfTrack*> midiTracks = m_midi.get_tracks();

        bool foundUsable = false;
        m_length = 0;
        
        for (auto midiTrack : midiTracks)
        {
            TrackType type = get_track_type(midiTrack->name);
            if (type == TrackType::Guitar)
            {
                // Add all difficulties for this track
                add(type, Difficulty::Expert);
                add(type, Difficulty::Hard);
                add(type, Difficulty::Medium);
                add(type, Difficulty::Easy);
                foundUsable = true;
            }

            // find the longest midi track
            if (m_length < midiTrack->endTickTime)
            {
                m_length = midiTrack->endTickTime;
            }
        }

        const ORCore::TempoTrack &tempoTrack = *m_midi.get_tempo_track();

        int32_t lastQnLength;

        for (auto &eventOrder : tempoTrack.tempoOrdering)
        {

            if (eventOrder.type == ORCore::TtOrderType::TimeSignature)
            {
                auto &ts = tempoTrack.timeSignature[eventOrder.index];
                m_tempoTrack.add_time_sig_event(ts.numerator, ts.denominator, ts.thirtySecondPQN/8.0, m_midi.pulsetime_to_abstime(ts.info.info.pulseTime), ts.info.info.pulseTime);
                logger->debug(_("Time signature change recieved at time {} {}/{}"), m_midi.pulsetime_to_abstime(ts.info.info.pulseTime), ts.numerator, ts.denominator);
            }
            else if (eventOrder.type == ORCore::TtOrderType::Tempo)
            {
                auto &tempo = tempoTrack.tempo[eventOrder.index];
                lastQnLength = tempo.qnLength;
                m_tempoTrack.add_tempo_event(tempo.qnLength, tempo.absTime, tempo.info.info.pulseTime);
                logger->debug(_("Tempo change recieved at time {} {}"), tempo.absTime, tempo.qnLength);
            } 
        }

        m_tempoTrack.add_tempo_event(lastQnLength, m_midi.pulsetime_to_abstime(m_length), m_length); // add final tempo change for bar barking purposes.
        m_tempoTrack.mark_bars();

        if (!foundUsable)
        {
            throw std::runtime_error("Invalid song format.");
        }

        logger->debug(_("Song loaded"));

        return false;
    }

    void Song::load_track(TrackInfo& trackInfo)
    {

        logger->debug(_("Loading Track {} {}"), track_type_to_name(trackInfo.type), diff_type_to_name(trackInfo.difficulty));

        std::vector<ORCore::SmfTrack*> midiTracks = m_midi.get_tracks();

        ORCore::SmfTrack* midiTrack = nullptr;

        // Find midi track
        for (ORCore::SmfTrack* _midiTrack : midiTracks)
        {
            TrackType type = get_track_type(_midiTrack->name);
            if (type == trackInfo.type)
            {
                midiTrack = _midiTrack;
            }
        }

        // If we didnt find a track skip it.
        if (midiTrack == nullptr)
        {
            return;
        }

        m_tracks.emplace_back(this, trackInfo);
        auto &track = m_tracks.back();

        for (auto &midiEvent : midiTrack->midiEvents)
        {
            // Handle velocity = 0 to turn notes off
            if (midiEvent.message == ORCore::NoteOn && midiEvent.data2 == 0)
            {
                midiEvent.message = ORCore::NoteOff;
            }

            if (midiEvent.message == ORCore::NoteOn || midiEvent.message == ORCore::NoteOff)
            {
                // To reduce code duplication use the same codepath for notes on/off but convert event types to bool.
                bool noteOn = false;
                if (midiEvent.message == ORCore::NoteOn)
                {
                    noteOn = true;
                }

                NoteType note = midi_to_note_type(trackInfo.difficulty, midiEvent.data1);

                double time = m_midi.pulsetime_to_abstime(midiEvent.info.pulseTime);

                switch (note)
                {
                    // Event markers
                    case NoteType::Solo:
                        track.set_event(EventType::solo, time, noteOn);
                        break;
                    case NoteType::Drive:
                        track.set_event(EventType::drive, time, noteOn);
                        break;

                    // Track Notes
                    case NoteType::Green:
                    case NoteType::Red:
                    case NoteType::Yellow:
                    case NoteType::Blue:
                    case NoteType::Orange:
                        track.add_note(note, time, midiEvent.info.pulseTime, noteOn);
                        break;
                    default: break;
                }
            }
        }
    }

    // Load all tracks
    void Song::load_tracks()
    {
        for (auto &trackInfo : m_tracksInfo)
        {
            load_track(trackInfo);
        }
        logger->debug(_("{} Tracks processed"), m_tracks.size());
    }

    std::vector<Track> *Song::get_tracks()
    {
        return &m_tracks;
    }

    std::vector<TrackInfo> &Song::get_track_info()
    {
        return m_tracksInfo; 
    }

    TempoTrack *Song::get_tempo_track()
    {
        return &m_tempoTrack;
    };

    int16_t Song::get_divison()
    {
        return m_midi.get_header()->division;
    }

    double Song::length()
    {
        return m_midi.pulsetime_to_abstime(m_length);
    }

    void Song::start()
    {
        m_songTimer.reset();
        m_audioOut.start();
        m_logger->info("Song started");
    }

    double Song::get_song_time()
    {
        // TODO - there is likely a better place for this...
        // Basically the plan here is have an update method.
        double tick = m_songTimer.tick();
        double time = m_songTimer.get_current_time();
        if (time < m_pauseTime && tick > 0.0)
        {
            m_songOgg.set_pause(false);
        }
        return time;
    }

    double Song::get_audio_time()
    {
        return m_songOgg.get_time();
    }

    void Song::set_pause(bool pause)
    {
        double time = m_songTimer.set_pause(pause);
        if (time > m_pauseTime)
        {
            m_pauseTime = time;
        }
        m_songTimer.set_resume_target(m_pauseTime-1.5, 2.0);
        if (pause)
        {
            m_songOgg.set_pause(true);
            m_songOgg.seek(m_pauseTime-1.5);
        }
    }

} // namespace ORGame
