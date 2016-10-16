#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <sstream>
#include <stdexcept>
#include <cmath>

#include "smf.hpp"
#include "parseutils.hpp"

namespace ORCore
{
    uint32_t SmfReader::readVarLen()
    {
        uint8_t c = ORCore::read_type<uint8_t>(m_smfFile);
        uint32_t value = static_cast<uint32_t>(c & 0x7F);

        if (c & 0x80) {

            do {
                c = ORCore::read_type<uint8_t>(m_smfFile);
                value = (value << 7) + (c & 0x7F);

            } while (c & 0x80);

        }
        return value;
    }

    void SmfReader::readMidiEvent(SmfEventInfo &event)
    {

        MidiEvent midiEvent;
        midiEvent.info = event;
        midiEvent.message = static_cast<uint8_t>(event.status & 0xF0);
        midiEvent.channel = static_cast<uint8_t>(event.status & 0xF);
        midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile);
        midiEvent.data2 = ORCore::read_type<uint8_t>(m_smfFile);

        m_currentTrack->midiEvents.push_back(midiEvent);
    }

    void SmfReader::readMetaEvent(SmfEventInfo &event)
    {
        event.type = static_cast<MidiMetaEvent>(ORCore::read_type<uint8_t>(m_smfFile));
        uint32_t length = readVarLen();

        // In the cases where we dont implement an event type log it, and its data.
        switch(event.type)
        {
            case meta_SequenceNumber:
            {
                auto sequenceNumber = ORCore::read_type<uint16_t>(m_smfFile);
                m_logger->trace("Sequence Number {}", sequenceNumber);
                break;
            }
            case meta_Text:
            case meta_Copyright:
            case meta_InstrumentName:
            case meta_Lyrics:
            case meta_Marker:
            case meta_CuePoint:
            // RP-019 - SMF Device Name and Program Name Meta Events
            case meta_ProgramName:
            case meta_DeviceName:
            // The midi spec says the following text events exist and act the same as meta_Text.
            case meta_TextReserved3:
            case meta_TextReserved4:
            case meta_TextReserved5:
            case meta_TextReserved6:
            case meta_TextReserved7:
            case meta_TextReserved8:
            {
                auto textData = std::make_unique<char[]>(length+1);
                textData[length] = '\0';
                ORCore::read_type<char>(m_smfFile, textData.get(), length);
                TextEvent text {event, textData.get()};
                break;
            }
            case meta_TrackName:
            {
                auto textData = std::make_unique<char[]>(length+1);
                textData[length] = '\0';

                ORCore::read_type<char>(m_smfFile, textData.get(), length);
                m_currentTrack->name = std::string(textData.get());
                break;
            }
            case meta_MIDIChannelPrefix: {
                auto midiChannel = ORCore::read_type<uint8_t>(m_smfFile);
                m_logger->trace("Midi Channel {}", midiChannel);
                break;
            }
            case meta_EndOfTrack:
            {
                // TODO - We might be able to use this for some purpose.
                // Actually yeah, this will be needed for proper bpm marking.
                // That way we can mark bpm until the end of a track
                m_logger->trace("End of Track {}", m_currentTrack->name);
                break;
            }
            case meta_Tempo:
            {
                uint32_t qnLength = read_type<uint32_t>(m_smfFile, 3);
                double ppqn = qnLength/static_cast<double>(m_header.division);
                m_currentTrack->tempo.push_back({event, qnLength, ppqn});
                if (m_tempoTrack == nullptr || m_header.format != smfType1) {
                    m_tempoTrack = m_currentTrack;
                }
                break;
            }
            case meta_TimeSignature:  // TODO - Implement this...
            {
                TimeSignatureEvent tsEvent;
                tsEvent.numerator = ORCore::read_type<uint8_t>(m_smfFile); // 4 default
                tsEvent.denominator = std::pow(2, ORCore::read_type<uint8_t>(m_smfFile)); // 4 default

                // This should be used to scale each beat for compound time signatures.
                tsEvent.clocksPerBeat = ORCore::read_type<uint8_t>(m_smfFile); // Standard is 24

                // The number of 1/32nd notes per quarter note not quite sure of its use yet.
                tsEvent.thirtySecondPQN = ORCore::read_type<uint8_t>(m_smfFile); // 8 default

                m_logger->trace("Time signature  {}/{} CPC: {} TSPQN: {}",
                                    tsEvent.numerator,
                                    tsEvent.denominator,
                                    tsEvent.clocksPerBeat,
                                    tsEvent.thirtySecondPQN);

                m_currentTrack->timeSigEvents.push_back(tsEvent);
                if (m_timeSigTrack == nullptr || m_header.format != smfType1) {
                    m_timeSigTrack = m_currentTrack;
                }

                break;
            }
            // These are mainly here to just represent them existing :P
            case meta_MIDIPort:  // obsolete no longer used.
            case meta_SMPTEOffset: // Not currently implemented, maybe someday.
            case meta_KeySignature: // Not very useful for us
            case meta_XMFPatchType: // probably not used
            case meta_SequencerSpecific:
            default:
            {
                m_logger->info("Unused event type {}.", event.type);
                m_smfFile.seekg(length, std::ios::cur);
                break;
            }
        }
    }

    void SmfReader::readSysExEvent(SmfEventInfo &event)
    {
        auto length = readVarLen();
        m_logger->info("sysex even at position {}", m_smfFile.tellg());
        m_smfFile.seekg(length, std::ios::cur);
    }

    double SmfReader::conv_abstime(uint32_t deltaPulses)
    {
        return deltaPulses * (m_currentTempoEvent->qnLength / (m_header.division * 1000000.0));
    }

    void SmfReader::readEvents(int chunkEnd)
    {
        uint32_t pulseTime = 0;
        uint8_t prevStatus = 0;
        double currentRunningTimeSec = 0;

        while (m_smfFile.tellg() < chunkEnd)
        {
            SmfEventInfo event;

            event.deltaPulses = readVarLen();

            // DO NOT use this for time calculations.
            // You must convert each deltaPulse to a time
            // within the currently active tempo.
            pulseTime += event.deltaPulses;

            event.pulseTime = pulseTime;

            if (pulseTime == 0) {
                event.absTime = 0.0;
            } else {
                currentRunningTimeSec += conv_abstime(event.deltaPulses);
                event.absTime = currentRunningTimeSec;
            }
            auto status = ORCore::peek_type<uint8_t>(m_smfFile);

            if (status == status_MetaEvent) {
                prevStatus = 0; // reset running status
                event.status = ORCore::read_type<uint8_t>(m_smfFile);
                readMetaEvent(event);
            } else if (status == status_SysexEvent || status == status_SysexEvent2) {
                prevStatus = 0;  // reset running status
                event.status = ORCore::read_type<uint8_t>(m_smfFile);
                readSysExEvent(event);
            } else {
                if ((status & 0xF0) >= 0x80) {
                    event.status = ORCore::read_type<uint8_t>(m_smfFile);
                } else {
                    event.status = prevStatus;
                }
                readMidiEvent(event);
                prevStatus = event.status;
            }

            if (pulseTime != 0 && (m_tempoTrack == nullptr || m_tempoTrack->tempo.size() == 0)) {

                m_logger->info("No tempo change at deltatime 0 setting default of 120 BPM.");

                // We construct a new tempo event that will have a default
                // equivelent to 120 BPM the same thing will need to be done
                // for the time signature meta event.
                SmfEventInfo tempoEvent;
                tempoEvent.deltaPulses = 0;
                tempoEvent.type = meta_Tempo;
                tempoEvent.pulseTime = 0;
                tempoEvent.absTime = 0.0;

                if (m_tempoTrack == nullptr) {
                    if (m_header.format != smfType1) {
                        m_tempoTrack = m_currentTrack;
                    } else {
                        m_tempoTrack = &m_tracks.front();
                    }
                }

                m_tempoTrack->tempo.push_back({tempoEvent, 500000}); // last value is ppqn
            }
            if (pulseTime != 0 && (m_tempoTrack == nullptr || m_timeSigTrack->timeSigEvents.size() == 0)) {

                m_logger->info("No time signature change at deltatime 0 setting default of 4/4.");

                TimeSignatureEvent tsEvent;
                tsEvent.numerator = 4;
                tsEvent.denominator = 4;
                tsEvent.clocksPerBeat = 24;
                tsEvent.thirtySecondPQN = 8;

                if (m_timeSigTrack == nullptr) {
                    if (m_header.format != smfType1) {
                        m_timeSigTrack = m_currentTrack;
                    } else {
                        m_timeSigTrack = &m_tracks.front();
                    }
                }

                m_timeSigTrack->timeSigEvents.push_back(tsEvent);
            }
            if (pulseTime != 0 || (m_tempoTrack != nullptr && m_tempoTrack->tempo.size() != 0)) {
                m_currentTempoEvent = getLastTempoIdViaPulses(pulseTime);
            }
        }
        m_currentTrack->seconds = static_cast<float>(currentRunningTimeSec);
    }

    void SmfReader::readFile()
    {
        // seek to the end in order to get the fileSize.
        m_smfFile.seekg(0, std::ios::end);
        int fileEnd = static_cast<int>(m_smfFile.tellg());

        // Now we go back to the beginning and begin reading the file
        m_smfFile.seekg(0, std::ios::beg);

        int fileStart = static_cast<int>(m_smfFile.tellg());
        int filePos = fileStart;
        int fileRemaining = fileEnd;

        SmfChunkInfo chunk;

        // set the intial chunk starting position at the beginning of the file.
        int chunkStart = fileStart;

        // The chunk end will be calculated after a chunk is loaded.
        int chunkEnd = 0;

        int trackChunkCount = 0;

        // We could loop through the number of track chunks given in the header.
        // However if there are any unknown chunk types inside the midi file
        // this will likely break. So we just loop until we hit the end of the
        // file instead...
        while (filePos < fileEnd)
        {

            ORCore::read_type<char>(m_smfFile, chunk.chunkType, 4);
            chunk.length = ORCore::read_type<uint32_t>(m_smfFile);
            chunkEnd = chunkStart + (8 + chunk.length); // 8 is the length of the type + length fields

            m_logger->trace("chunk of type {} detected.", chunk.chunkType);
            // MThd chunk is only in the beginning of the file.
            if (chunkStart == fileStart && strcmp(chunk.chunkType, "MThd") == 0) {
                // Load header chunk
                m_header.info = chunk;
                m_header.format = ORCore::read_type<uint16_t>(m_smfFile);
                m_header.trackNum = ORCore::read_type<uint16_t>(m_smfFile);
                m_header.division = ORCore::read_type<int16_t>(m_smfFile);

                // Make sure we reserve enough space for m_tracks just in-case.
                m_tracks.reserve(sizeof(SmfTrack) * m_header.trackNum);

                if (m_header.format == smfType0 && m_header.trackNum != 1) {
                    throw std::runtime_error("Not a valid type 0 midi.");
                }

                // TODO - For completionist reasons eventually add support for this.
                if ((m_header.division & 0x8000) != 0) {
                    throw std::runtime_error("SMPTE time division not supported");
                }

            } else if (strcmp(chunk.chunkType, "MTrk") == 0) {
                trackChunkCount += 1;
                m_tracks.emplace_back();
                m_currentTrack = &m_tracks.back();
                readEvents(chunkEnd);

            } else {
                m_logger->warn("Non-standard chunk of type {} detected, skipping.", chunk.chunkType);
            }

            filePos = chunkEnd;
            chunkStart = filePos;

            // Make sure that we are in the correct location in the chunk
            // If not seek to the correct location and output an error in the log.
            if (static_cast<int>(m_smfFile.tellg()) != filePos) {
                m_logger->warn("Offset for chunk '{}' incorrect, seeking to correct location.", chunk.chunkType);
                m_logger->warn("Offset difference. expected: '{}' actual: '{}'", m_smfFile.tellg(), filePos);
                //m_smfFile.seekg(filePos);
            }
            fileRemaining = (fileEnd-filePos);
            if (fileRemaining != 0 && fileRemaining <= 8) {
                m_logger->warn("To few bytes remaining in midi for another track, this is likely a bug.");
            }
        }
        if (trackChunkCount != m_header.trackNum) {
            m_logger->warn("Track chunk count does not match header.");
        }
        m_logger->info("End of MIDI reached.");

    }

    SmfReader::SmfReader(std::string filename)
    :m_tempoTrack(nullptr), m_timeSigTrack(nullptr)
    {
        m_logger = spdlog::get("default");
        m_logger->info("Loading MIDI");

        std::ifstream smfFile(filename, std::ios_base::ate | std::ios_base::binary);

        if (smfFile) {
            std::string contents;
            auto size = smfFile.tellg();
            smfFile.seekg(0, std::ios::beg);
            contents.resize(size);
            smfFile.read(&contents[0], size);
            smfFile.close();
            m_smfFile.str(contents);
        } else {
            throw std::runtime_error("Failed to load MIDI file.");
        }

        readFile();
        // Release midifile data after we are finished reading.
        m_smfFile.str( std::string() );
        m_smfFile.clear();
    }

    TempoEvent* SmfReader::getLastTempoIdViaPulses(uint32_t pulseTime)
    {
        static unsigned int value = 0;
        std::vector<TempoEvent> &tempos = m_tempoTrack->tempo;

        static uint32_t lastPulseTime = 0;

        // Ignore the cached last tempo value if the new pulse time is older.
        if (lastPulseTime > pulseTime) {
            //m_logger->trace("Reset tempo id from: {}", value);
            value = 0;
        }

        for (unsigned int i = 0; i < tempos.size(); i++) {
            // Restore tempo to previous value
            if (i < value) {
                //m_logger->trace("Restored tempo id to: {}", value);
                i = value;
            }
            if (tempos[i].info.pulseTime >=pulseTime) {
                value = i;
                lastPulseTime = pulseTime;
                return &tempos[i];
            }
        }
        // return last value if nothing else is found
        return &tempos.back();

    }

    std::vector<SmfTrack*> SmfReader::getTracks()
    {
        std::vector<SmfTrack*> tracks;

        for (auto &track : m_tracks) {
            tracks.push_back(&track);
        }
        return tracks;
    }
}
