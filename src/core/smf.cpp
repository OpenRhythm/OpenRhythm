#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <sstream>
#include <stdexcept>

#include "smf.hpp"
#include "parseutils.hpp"

namespace MgCore
{
    uint32_t SmfReader::readVarLen()
    {
        uint8_t c = MgCore::read_type<uint8_t>(*m_smfFile);
        uint32_t value = c & 0x7F;

        if (c & 0x80) {

            do {
                c = MgCore::read_type<uint8_t>(*m_smfFile);
                value = (value << 7) + (c & 0x7F);

            } while (c & 0x80);

        }
        return value;
    }

    void SmfReader::readMidiEvent(SmfEventInfo &event)
    {

        MidiEvent midiEvent;
        midiEvent.info = event;
        midiEvent.message = event.status & 0xF0;
        midiEvent.channel = event.status & 0xF;
        midiEvent.data1 = MgCore::read_type<uint8_t>(*m_smfFile);
        midiEvent.data2 = MgCore::read_type<uint8_t>(*m_smfFile);

        m_currentTrack->midiEvents.push_back(midiEvent);
    }

    void SmfReader::readMetaEvent(SmfEventInfo &event)
    {
        event.type = static_cast<MidiMetaEvent>(MgCore::read_type<uint8_t>(*m_smfFile));
        uint32_t length = readVarLen();

        // In the cases where we dont implement an event type log it, and its data.
        switch(event.type)
        {
            case meta_SequenceNumber:
            {
                auto sequenceNumber = MgCore::read_type<uint16_t>(*m_smfFile);
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
                MgCore::read_type<char>(*m_smfFile, textData.get(), length);
                TextEvent text {event, textData.get()};
                break;
            }
            case meta_TrackName:
            {
                auto textData = std::make_unique<char[]>(length+1);
                textData[length] = '\0';

                MgCore::read_type<char>(*m_smfFile, textData.get(), length);
                m_currentTrack->name = std::string(textData.get());
                break;
            }
            case meta_MIDIChannelPrefix: {
                auto midiChannel = MgCore::read_type<uint8_t>(*m_smfFile);
                m_logger->trace("Midi Channel {}", midiChannel);
                break;
            }
            case meta_EndOfTrack:
            {
                // TODO - We might be able to use this for some purpose.
                m_logger->trace("End of Track {}", m_currentTrack->name);
                break;
            }
            case meta_Tempo:
            {
                uint32_t qnLength = read_type<uint32_t>(*m_smfFile, 3);
                m_currentTrack->tempo.push_back({event, qnLength});
                break;
            }
            case meta_TimeSignature:  // TODO - Implement this...
            {
                auto numerator = MgCore::read_type<uint8_t>(*m_smfFile);
                auto denominator = MgCore::read_type<uint8_t>(*m_smfFile);

                auto clocksPerClick = MgCore::read_type<uint8_t>(*m_smfFile);
                // The number of 1/32nd notes per quarter note
                auto thirtySecondPQN = MgCore::read_type<uint8_t>(*m_smfFile);

                m_logger->trace("Time signature {}", m_currentTrack->name);
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
                m_smfFile->seekg(length, std::ios::cur);
                break;
            }
        }
    }


    void SmfReader::readSysExEvent(SmfEventInfo &event)
    {
        auto length = readVarLen();
        m_logger->info("sysex even at position {}", m_smfFile->tellg());
        m_smfFile->seekg(length, std::ios::cur);
    }

    void SmfReader::readEvents(int chunkEnd)
    {
        uint32_t pulseTime = 0;
        double runningTimeSec = 0.0;
        TempoEvent* currentTempoEvent;
        uint8_t prevStatus = 0;

        while (m_smfFile->tellg() < chunkEnd)
        {
            SmfEventInfo event;

            event.deltaPulses = readVarLen();

            pulseTime += event.deltaPulses;

            event.pulseTime = pulseTime;

            auto status = MgCore::peek_type<uint8_t>(*m_smfFile);

            if (status == status_MetaEvent) {
                prevStatus = 0; // reset running status
                event.status = MgCore::read_type<uint8_t>(*m_smfFile);
                readMetaEvent(event);
            } else if (status == status_SysexEvent || status == status_SysexEvent2) {
                prevStatus = 0;  // reset running status
                event.status = MgCore::read_type<uint8_t>(*m_smfFile);
                readSysExEvent(event);
            } else {

                if ((status & 0xF0) >= 0x80) {
                    event.status = MgCore::read_type<uint8_t>(*m_smfFile);
                } else {
                    event.status = prevStatus;
                }
                readMidiEvent(event);
                prevStatus = event.status;
            }
            if (m_tempoTrack->tempo.size() != 0) {
                currentTempoEvent = getLastTempoIdViaPulses(pulseTime);
                runningTimeSec += event.deltaPulses * ((currentTempoEvent->qnLength / m_header.division) / 1000000.0);
            } else if (pulseTime != 0) {
                // We construct a new tempo event that will have a default
                // equivelent to 120 BPM the same thing will need to be done
                // for the time signature meta event.
                SmfEventInfo tempoEvent;
                tempoEvent.deltaPulses = 0;
                tempoEvent.type = meta_Tempo;
                tempoEvent.pulseTime = 0;
                tempoEvent.absTime = 0.0;

                m_tempoTrack->tempo.push_back({tempoEvent, 500000});
            }
        }
        m_currentTrack->seconds = static_cast<float>(runningTimeSec);
    }

    void SmfReader::readFile()
    {
        // we start the file opened at the end in order to get the fileSize.
        int fileEnd = static_cast<int>(m_smfFile->tellg());

        // Now we go back to the beginning and begin reading the file
        m_smfFile->seekg(0, std::ios::beg);

        int fileStart = static_cast<int>(m_smfFile->tellg());
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

            MgCore::read_type<char>(*m_smfFile, chunk.chunkType, 4);
            chunk.length = MgCore::read_type<uint32_t>(*m_smfFile);
            chunkEnd = chunkStart + (8 + chunk.length); // 8 is the length of the type + length fields

            // MThd chunk is only in the beginning of the file.
            if (chunkStart == fileStart && strcmp(chunk.chunkType, "MThd") == 0) {
                // Load header chunk
                m_header.info = chunk;
                m_header.format = MgCore::read_type<uint16_t>(*m_smfFile);
                m_header.trackNum = MgCore::read_type<uint16_t>(*m_smfFile);
                m_header.division = MgCore::read_type<int16_t>(*m_smfFile);

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

                // Make sure that for midi type 0 and type 2 the tempoTrack is
                // changing with m_currentTrack.
                if (m_header.format != smfType1 || trackChunkCount == 1) {
                    m_tempoTrack = m_currentTrack;
                }

                readEvents(chunkEnd);

            } else {
                m_logger->warn("Non-standard chunk of type {} detected, skipping.", chunk.chunkType);
            }

            filePos = chunkEnd;
            chunkStart = filePos;

            // Make sure that we are in the correct location in the chunk
            // If not seek to the correct location and output an error in the log.
            if (static_cast<int>(m_smfFile->tellg()) != filePos) {
                m_logger->warn("Offset for chunk '{}' incorrect, seeking to correct location.", chunk.chunkType);
                m_smfFile->seekg(filePos);
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
    {
        m_logger = spdlog::get("default");
        m_smfFile = std::make_unique<std::ifstream>(filename, std::ios_base::ate | std::ios_base::binary);

        if (*m_smfFile) {
            readFile();
            m_smfFile->close();
        } else {
            throw std::runtime_error("Failed to load MIDI file.");
        }
    }

    // TODO - rework so this isn't called until after a note has arrived
    // or the first tempo meta event has arrived. Related to not having a
    // default tempo of 120 bpm as the midi spec defines.
    TempoEvent* SmfReader::getLastTempoIdViaPulses(uint32_t pulseTime)
    {
        SmfTrack &tempoTrack = m_tracks.front();
        std::vector<TempoEvent> &tempos = tempoTrack.tempo;

        if (tempos.size() == 0) {
            m_logger->info("Too early no tempo changes");
            return nullptr;
        }

        for (auto &tempo : tempos) {
            if (pulseTime >= tempo.info.pulseTime) {
                return &tempo;
            }
        }
        return nullptr;
    }

    //TempoEvent SmfReader::getTempoViaId(int id)
    //{
    //    tempoTrack = m_tracks[0]
    //    return tempoTrack->tempo[id]
    //}

    std::vector<SmfTrack*> SmfReader::getTracks()
    {
        std::vector<SmfTrack*> tracks;

        for (auto &track : m_tracks) {
            tracks.push_back(&track);
        }
        return tracks;
    }
}
