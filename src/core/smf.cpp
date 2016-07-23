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
        uint8_t c = MgCore::read_type<uint8_t>(*m_smf);
        uint32_t value = c & 0x7F;

        if (c & 0x80) {

            do {
                c = MgCore::read_type<uint8_t>(*m_smf);
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
        midiEvent.data1 = MgCore::read_type<uint8_t>(*m_smf);
        midiEvent.data2 = MgCore::read_type<uint8_t>(*m_smf);

        //std::cout << "Channel " << +midiEvent.channel << std::endl;

        m_currentTrack->midiEvents.push_back(midiEvent);
    }

    void SmfReader::readMetaEvent(SmfEventInfo &event)
    {
        event.type = static_cast<MidiMetaEvent>(MgCore::read_type<uint8_t>(*m_smf));
        uint32_t length = readVarLen();

        switch(event.type)
        {
            case meta_SequenceNumber:
            {
                auto sequenceNumber = MgCore::read_type<uint16_t>(*m_smf);
                //std::cout << "Sequence Number: " << " " << std::hex << sequenceNumber << std::endl;
                break;
            }
            case meta_Text:
            case meta_Copyright:
            case meta_InstrumentName:
            case meta_Lyrics:
            case meta_Marker:
            case meta_CuePoint:
            // The midi spec says the following text events exist and act the same as meta_Text.
            case meta_TextReserved1:
            case meta_TextReserved2:
            case meta_TextReserved3:
            case meta_TextReserved4:
            case meta_TextReserved5:
            case meta_TextReserved6:
            case meta_TextReserved7:
            case meta_TextReserved8:
            {
                auto textData = std::make_unique<char[]>(length+1);
                textData[length] = '\0';
                MgCore::read_type<char>(*m_smf, textData.get(), length);
                TextEvent text {event, textData.get()};
                break;
            }
            case meta_TrackName:
            {
                auto textData = std::make_unique<char[]>(length+1);
                textData[length] = '\0';

                MgCore::read_type<char>(*m_smf, textData.get(), length);
                m_currentTrack->name = std::string(textData.get());
                break;
            }
            case meta_MIDIChannelPrefix: {
                auto midiChannel = MgCore::read_type<uint8_t>(*m_smf);
                //std::cout << "Midi Channel " << midiChannel << std::hex << std::endl;
                break;
            }
            case meta_EndOfTrack:
            {
                //std::cout << "End of Track " << std::endl;
                break;
            }
            case meta_Tempo:
            {
                uint32_t bpmChange = read_type<uint32_t>(*m_smf, 3);
                //std::cout << "Tempo Change " << 60000000.0 / bpmChange << "BPM" << std::endl;
                TempoEvent tempo {event, bpmChange};
                m_currentTrack->tempo.push_back(tempo);
                break;
            }
            case meta_SMPTEOffset:
            case meta_TimeSignature:  // TODO - Implement this...
            case meta_KeySignature:  // Not very useful for us
            case meta_XMFPatchType: // probably not used
            case meta_SequencerSpecific:
            {
                m_smf->seekg(length, std::ios::cur);
                break;
            }
            default:
            {
                m_smf->seekg(length, std::ios::cur);
                break;
            }
        }
    }


    void SmfReader::readSysExEvent(SmfEventInfo &event)
    {
        auto length = readVarLen();
        std::cout << "sysex event " << m_smf->tellg() << std::endl;
        m_smf->seekg(length, std::ios::cur);
    }

    void SmfReader::readFile()
    {

        int fileEnd = static_cast<int>(m_smf->tellg());
        m_smf->seekg(0, std::ios::beg);
        int fileBeg = static_cast<int>(m_smf->tellg());

        int chunkStart = fileBeg;
        int pos = 0;

        SmfChunkInfo chunk;

        MgCore::read_type<char>(*m_smf, chunk.chunkType, 4);
        chunk.length = MgCore::read_type<uint32_t>(*m_smf);

        if (strcmp(chunk.chunkType, "MThd") == 0)  {

            m_header.info = chunk;
            m_header.format = MgCore::read_type<uint16_t>(*m_smf);
            m_header.trackNum = MgCore::read_type<uint16_t>(*m_smf);
            m_header.division = MgCore::read_type<int16_t>(*m_smf);

            // seek to the end of the chunk
            // 8 is the length of the type plus length fields
            pos = chunkStart + (8 + chunk.length);

            m_smf->seekg(pos);

            if (m_header.format == smfType0 && m_header.trackNum != 1) {
                throw std::runtime_error("Not a valid type 0 midi.");
            }
        } else {
            throw std::runtime_error("Not a Standard MIDI file.");
        }

        if ((m_header.division & 0x8000) != 0) {
            throw std::runtime_error("SMPTE division not supported");
        }

        for (int i = 0; i < m_header.trackNum; i++) {

            MgCore::read_type<char>(*m_smf, chunk.chunkType, 4);
            chunk.length = MgCore::read_type<uint32_t>(*m_smf);


            m_pulseTime = 0;
            double runningTimeSec = 0.0;
            TempoEvent* currentTempoEvent;

            // seek to the end of the chunk
            // 8 is the length of the type plus length fields
            chunkStart = pos;
            pos = chunkStart + (8 + chunk.length);

            if (strcmp(chunk.chunkType, "MTrk") == 0) {

                uint8_t prevStatus;

                auto track = std::make_unique<SmfTrack>();
                m_currentTrack = track.get();

                m_tracks.push_back(std::move(track));


                while (m_smf->tellg() < pos)
                {
                    SmfEventInfo event;

                    event.deltaPulses = readVarLen();

                    m_pulseTime += event.deltaPulses;

                    event.pulseTime = m_pulseTime;

                    auto status = MgCore::peek_type<uint8_t>(*m_smf);

                    if (status == status_MetaEvent) {
                        event.status = MgCore::read_type<uint8_t>(*m_smf);
                        readMetaEvent(event);
                    } else if (status == status_SysexEvent || status == status_SysexEvent2) {
                        event.status = MgCore::read_type<uint8_t>(*m_smf);
                        readSysExEvent(event);
                    } else {

                        if ((status & 0xF0) >= 0x80) {
                            event.status = MgCore::read_type<uint8_t>(*m_smf);
                        } else {
                            event.status = prevStatus;
                        }
                        readMidiEvent(event);
                        prevStatus = event.status;

                    }

                    currentTempoEvent = getLastTempoIdViaPulses(m_pulseTime);
                    if (currentTempoEvent != nullptr) {
                        runningTimeSec += event.deltaPulses * ((currentTempoEvent->qnLength / m_header.division) / 1000000.0);
                    } else {
                         std::cout << event.deltaPulses << std::endl;
                    }

                }
                //std::cout << m_currentTrack->midiEvents.size() << std::endl;
                m_currentTrack->seconds = static_cast<float>(runningTimeSec);
                std::cout << "Track length: " << m_currentTrack->seconds << std::endl;


            } else {
                //std::cout << "Supported track chunk not found." << std::endl;
            }

            m_smf->seekg(pos);
        }

        if (pos == fileEnd) {
            //std::cout << "End of MIDI reached." << std::endl;
        } else {
            //std::cout << "Warning: Reached end of last track chunk, there is possibly data located outside of a track." << std::endl;
        }

    }

    SmfReader::SmfReader(std::string filename)
    {
        m_smf = std::make_unique<std::ifstream>(filename, std::ios_base::ate | std::ios_base::binary);

        // TODO - find cleaner way of implementing default the 120 BPM
        // m_currentTempo = 500000; // default 120 BPM

        if (*m_smf) {
            readFile();
            m_smf->close();
        } else {
            throw std::runtime_error("Failed to load MIDI file.");
        }
    }

    TempoEvent* SmfReader::getLastTempoIdViaPulses(uint32_t pulseTime)
    {
        auto tempoTrack = *m_tracks[0];
        auto tempos = tempoTrack.tempo;

        if (tempos.size() == 0) {
            std::cout << "Error too early no tempo changes" << std::endl;
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
            tracks.push_back(track.get());
        }
        return tracks;
    }
}
