#pragma once

#include <string>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace MgCore
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

        // The midi spec says the following text events exist and act the same as meta_Text.
        meta_TextReserved1,
        meta_TextReserved2,
        meta_TextReserved3,
        meta_TextReserved4,
        meta_TextReserved5,
        meta_TextReserved6,
        meta_TextReserved7,
        meta_TextReserved8, // 0x0F

        meta_MIDIChannelPrefix = 0x20,
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
        uint8_t status;
        uint32_t deltaTime;
    };

    struct MidiEvent
    {
    	SmfEventInfo info;
    	uint8_t message;
    	uint8_t channel;
        uint8_t data1;
        uint8_t data2;
    };

    struct TextEvent
    {
        SmfEventInfo info;
    };

    struct TempoEvent
    {
        SmfEventInfo info;
        uint32_t qnLength; // Length of a quarter note in microseconds.
        uint32_t pulseTime; // Time in pulses of Tempo event
    };

    struct TimeSigEvent
    {
        SmfEventInfo info;
    };

    struct SmfTrack
    {
    	std::string name;
        float seconds;
    	std::vector<MidiEvent> midiEvents;
    	std::vector<TextEvent> textEvents;
    	std::vector<TempoEvent> tempo;
    	std::vector<TimeSigEvent> timeSigEvents;
    };

    class SmfReader
    {
    private:
        typedef std::unique_ptr<SmfTrack> t_SmfTrackPtr;
        std::vector<t_SmfTrackPtr> m_tracks;
        SmfHeaderChunk m_header;
        std::unique_ptr<std::ifstream> m_smf;
        SmfTrack *m_currentTrack;
        int m_currentTempo;
        uint32_t readVarLen();
        void readMidiEvent(SmfEventInfo &event);
        void readMetaEvent(SmfEventInfo &event);
        void readSysExEvent(SmfEventInfo &event);
        TempoEvent* getLastTempoIdViaPulses(uint32_t pulseTime);
        void readFile();

    public:
        SmfReader(std::string smfData);
        std::vector<SmfTrack*> getTracks();
    };
}
