// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include <vector>
#include <string.h>
#include <cmath>
#include "parseutils.hpp"

namespace ORCore
{
    enum MidiMetaEvent: uint8_t
    {
        meta_SequenceNumber = 0x00,
        meta_Text,
        meta_Copyright,
        meta_TrackName,
        meta_InstrumentName,
        meta_Lyrics,
        meta_Marker,
        meta_CuePoint,

        // RP-019 - SMF Device Name and Program Name Meta Events
        meta_ProgramName,
        meta_DeviceName,

        // The midi spec says the following text events exist and act the same as meta_Text.
        meta_TextReserved3,
        meta_TextReserved4,
        meta_TextReserved5,
        meta_TextReserved6,
        meta_TextReserved7,
        meta_TextReserved8, // 0x0F

        meta_MIDIChannelPrefix = 0x20,
        meta_MIDIPort = 0x21, // obsolete no longer used.
        meta_EndOfTrack = 0x2F,
        meta_Tempo = 0x51,
        meta_SMPTEOffset = 0x54,
        meta_TimeSignature = 0x58,
        meta_KeySignature = 0x59,
        meta_XMFPatchType = 0x60, // For completeness probably wont show up in midi
        meta_SequencerSpecific = 0x7F,
    };


    enum MidiEventStatus
    {
        status_MetaEvent = 0xFF,
        status_SysexEvent = 0xF0,
        status_SysexEvent2 = 0xF7,
    };

    enum MidiChannelMessage: uint8_t
    {
        NoteOff         = 0x80,
        NoteOn          = 0x90,
        KeyPressure     = 0xA0,
        ControlChange   = 0xB0,
        ProgramChange   = 0xC0,
        ChannelPressure = 0xD0,
        PitchBend       = 0xE0,
    };

    enum SmfType
    {
        smfType0,
        smfType1,
        smfType2,
    };

    struct SmfChunkInfo
    {
        char chunkType[5];
        uint32_t length;

        SmfChunkInfo()
        {
            chunkType[4] = '\0';
        }
    };

    struct SmfHeaderChunk
    {
        SmfChunkInfo info;
        uint16_t format;
        uint16_t trackNum;
        int16_t division;
    };

    struct SmfEventInfo
    {
        uint32_t deltaPulses; // number of pulses relative to the previous event
        uint32_t pulseTime; // Time in pulses from start to now
        uint8_t status;
    };

    struct MetaEvent
    {
        SmfEventInfo info;
        MidiMetaEvent type;
        uint32_t length;
    };

    // This is for storing currently unused meta events for
    // passthrough once we have a midi writer.
    struct MetaStorageEvent
    {
        MetaEvent event;
        std::vector<char> data;
    };

    struct SysexEvent
    {
        SmfEventInfo info;
        MidiMetaEvent type;
        uint32_t length;
    };

    struct MidiEvent
    {
        SmfEventInfo info;
        MidiChannelMessage message;
        uint8_t channel;
        uint8_t data1;
        uint8_t data2;
    };

    struct TextEvent
    {
        MetaEvent info;
        std::string text;
    };

    struct TempoEvent
    {
        MetaEvent info;

        // Length of a quarter note in microseconds.
        uint32_t qnLength;

        // Time in seconds from midi start to now
        double absTime;

        // Time per header division.
        double timePerTick;
    };

    struct TimeSignatureEvent
    {
        MetaEvent info;
        int numerator;
        int denominator;
        int clocksPerBeat;
        int thirtySecondPQN;
    };

    enum class TtOrderType
    {
        TimeSignature,
        Tempo
    };

    struct TteventIndex
    {
        TtOrderType type;
        int index;

    };

    struct SmfTrack
    { 
        std::string name;
        double endTime; // Track length
        std::vector<MidiEvent> midiEvents;
        std::vector<TextEvent> textEvents;
        std::vector<MetaStorageEvent> miscMeta;
    };

    struct TempoTrack
    {
        std::vector<TempoEvent> tempo;
        std::vector<TimeSignatureEvent> timeSignature;
        std::vector<TteventIndex> tempoOrdering;
    };

    class SmfReader
    {
    public:
        SmfReader(std::string smfData);
        std::vector<SmfTrack*> get_tracks();
        TempoTrack* get_tempo_track();
        double pulsetime_to_abstime(uint32_t pulseTime);
        SmfHeaderChunk* get_header();
        void release();

    private:
        FileBuffer m_smfFile;
        SmfHeaderChunk m_header;
        TempoTrack m_tempoTrack;
        std::vector<SmfTrack> m_tracks;

        SmfTrack *m_currentTrack;

        uint32_t read_var_len();
        void read_midi_event(const SmfEventInfo &event);
        void read_meta_event(const SmfEventInfo &event);
        void read_sysex_event(const SmfEventInfo &event);
        double delta_tick_to_delta_time(TempoEvent* tempo, uint32_t deltaPulses);
        void set_default_tempo_ts();
        TempoEvent* get_last_tempo_via_pulses(uint32_t pulseTime);
        void read_events(uint32_t chunkEnd);
        void read_file();

        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace ORCore
