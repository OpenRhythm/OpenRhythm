#include "config.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include "midi_song.hpp"

#include "vfs.hpp"

namespace MidiPlayer
{


    static std::shared_ptr<spdlog::logger> logger;


    /////////////////////////////////////
    // Track Class
    /////////////////////////////////////

    Track::Track()
    {
        logger = spdlog::get("default");
    }

    void Track::add_note(int noteValue, double time, bool on)
    {
        static std::vector<std::pair<int, int>> activeNotes;

        if (on) {
            int index = m_notes.size();
            activeNotes.emplace_back(noteValue, index);
            m_notes.push_back({time, 0.0, noteValue});
        } else {
            auto findFunc = [&](const auto& element)
            {
                return element.first == noteValue;
            };
            auto item = std::find_if( activeNotes.begin(), activeNotes.end(), findFunc);
            if (item != activeNotes.end())
            {
                auto &note = m_notes[item->second];
                note.length = time - note.time;
                activeNotes.erase(item);
            }
        }
    }

    // static unsigned int value = 0;
    // static uint32_t lastPulseTime = 0;

    // std::vector<TempoEvent> &tempos = m_tempoTrack->tempo;

    // // Ignore the cached last tempo value if the new pulse time is older.
    // if (lastPulseTime > pulseTime) {
    //     value = 0;
    // }

    // for (unsigned int i = value; i < tempos.size(); i++) {
    //     if (tempos[i].info.info.pulseTime >= pulseTime) {
    //         value = i;
    //         lastPulseTime = pulseTime;
    //         return &tempos[i];
    //     }
    // }
    // // return last value if nothing else is found
    // return &tempos.back();

    // std::vector<TrackNote*> Track::get_notes_in_frame(double start, double end)
    // {   

    //     static unsigned int value = 0;
    //     static double lastTime = 0.0;

    //     if (lastTime > start) {
    //         value = 0;
    //         lastTime = 0.0;
    //         logger->info("RESET");
    //     }

    //     bool setTime = false;

    //     std::vector<TrackNote*> notes;
    //     for (unsigned int i = value; i<m_notes.size(); i++) {
    //         auto *note = &m_notes[i];
    //         if (note->time >= start && note->time <= end) {
    //             if (setTime == false)
    //             {
    //                 value = i;
    //                 lastTime=start;
    //                 setTime=true;
    //             }
    //             notes.emplace_back( &m_notes[i] );
    //         } else if (note->time > end) {
    //             return notes;
    //         }
    //     }
    //     return notes;
    // }

    TrackView Track::get_notes_in_frame(double start, double end)
    {   
        int startIndex = 0;
        int endIndex = 0;
        for (unsigned int i = startIndex; i<m_notes.size(); i++) {
            double time = m_notes[i].time;
            if (time >= start && time <= end)
            {
                if (startIndex == 0) {
                    startIndex = i;
                }
            } else if (time > end) {
                endIndex = i;
                break;
            }
        }
        // for ( auto &note : m_notes) {
        //     if (note.time >= start && ) {
        //         notes.emplace_back( &note );
        //     } else if (note.time > end) {
        //         return notes;
        //     }
        // }
        return {&m_notes, startIndex, endIndex};
    }

    TrackView Track::get_notes()
    {
        return {&m_notes, 0, static_cast<int>(m_notes.size())};
    }

    /////////////////////////////////////
    // Song Class methods
    /////////////////////////////////////

    Song::Song()
    :m_midi("notes.mid")
    {
        logger = spdlog::get("default");
    }

    void Song::load()
    {


        std::vector<ORCore::SmfTrack*> midiTracks = m_midi.get_tracks();
        logger->info("tracks: {}", midiTracks.size());
        
        for (auto midiTrack : midiTracks) {
            Track track;
            for (auto &midiEvent : midiTrack->midiEvents) {
                try {
                    if (midiEvent.message == ORCore::NoteOn) {
                        track.add_note(midiEvent.data1, midiEvent.info.absTime, true);
                    } else if (midiEvent.message == ORCore::NoteOff) {
                        track.add_note(midiEvent.data1, midiEvent.info.absTime, false);
                    }
                } catch (std::out_of_range &err) {
                    continue;
                }
            }
            m_length = midiTrack->endTime;
            m_tracks.push_back(track);
        }

        logger->debug(_("{} Tracks processed"), m_tracks.size());
        logger->debug(_("Song loaded"));
    }

    std::vector<Track> *Song::get_tracks()
    {
        return &m_tracks;
    }

    double Song::length()
    {
        return m_length;
    };

} // namespace MidiPlayer
