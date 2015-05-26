#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

#include "smf.hpp"

uint32_t SmfReader::readVarLen()
{
    uint8_t c = read_type<uint8_t>(*m_smf);
    uint32_t value = c & 0x7F;

    if (c & 0x80) {

        do {
            c = read_type<uint8_t>(*m_smf);
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
    midiEvent.data1 = read_type<uint8_t>(*m_smf);
    midiEvent.data2 = read_type<uint8_t>(*m_smf);

    m_currentTrack->midiEvents.push_back(midiEvent);
}

void SmfReader::readMetaEvent(SmfEventInfo &event)
{
    auto metaType = read_type<uint8_t>(*m_smf);
    uint32_t len = readVarLen();

    switch(metaType)
    {
        case meta_SequenceNumber:
        {
            auto sequenceNumber = read_type<uint16_t>(*m_smf);
            std::cout << "Sequence Number: " << " " << std::hex << sequenceNumber << std::endl;
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
            char textData[128];
            textData[len] = '\0';

            read_type<char>(*m_smf, textData, len);
            std::cout << "text: " << +metaType << " "  << textData << std::endl;
            break;
        }
        case meta_TrackName:
        {
            char textData[128];
            textData[len] = '\0';

            read_type<char>(*m_smf, textData, len);
            m_currentTrack->name = textData;
            break;
        }
        case meta_MIDIChannelPrefix: {
            auto midiChannel = read_type<uint8_t>(*m_smf);
            std::cout << "Midi Channel " << midiChannel << std::hex << std::endl;
            break;
        }
        case meta_EndOfTrack:
        {
            std::cout << "End of Track " << std::endl;
            break;
        }
        case meta_Tempo:
        {
            uint8_t bpmcharr[3];
            read_type<uint8_t>(*m_smf, bpmcharr, 3);
            uint32_t bpmChange = (bpmcharr[0] << 16) | (bpmcharr[1] << 8)  | bpmcharr[2];
            std::cout << "Tempo Change " << bpmChange << std::endl;
            break;
        }
        case meta_SMPTEOffset:
        case meta_TimeSignature:
        case meta_KeySignature:
        case meta_SequencerSpecific:
        {
            m_smf->seekg(len, std::ios::cur);
        }
        default:
        {
            m_smf->seekg(len, std::ios::cur);
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

    int fileEnd = m_smf->tellg();
    m_smf->seekg(0, std::ios::beg);
    int fileBeg = m_smf->tellg();

    int chunkStart = fileBeg;
    int pos = 0;

    SmfChunkInfo chunk;
    
    read_type<char>(*m_smf, chunk.chunkType, 4);
    chunk.length = read_type<uint32_t>(*m_smf);

    if (strcmp(chunk.chunkType, "MThd") == 0)  {
        
        m_header.info = chunk;
        m_header.format = read_type<uint16_t>(*m_smf);
        m_header.trackNum = read_type<uint16_t>(*m_smf);
        m_header.division = read_type<int16_t>(*m_smf);

        // seek to the end of the chunk
        // 8 is the length of the type plus length fields
        pos = chunkStart + (8 + chunk.length);

        m_smf->seekg(pos);

        if (m_header.format == smfType0 && m_header.trackNum != 1) {
            std::cout << "Not a valid type 0 midi." << std::endl;
        }
    } else {
        std::cout << "File not a valid MIDI." << std::endl;
        return;
    }

    if (m_header.division & 0x8000 != 0) {
        std::cout << "SMPTE division mode is currently not supported" << std::endl;
        return;
    }

    for (int i = 0; i < m_header.trackNum; i++) {

        read_type<char>(*m_smf, chunk.chunkType, 4);
        chunk.length = read_type<uint32_t>(*m_smf);

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

                event.deltaTime = readVarLen();

                auto status = peek_type<uint8_t>(*m_smf);

                if (status == status_MetaEvent) {
                    event.status = read_type<uint8_t>(*m_smf);
                    readMetaEvent(event);
                } else if (status == status_SysexEvent || status == status_SysexEvent2) {
                    event.status = read_type<uint8_t>(*m_smf);
                    readSysExEvent(event);
                } else {

                    if ((status & 0xF0) >= 0x80) {
                        event.status = read_type<uint8_t>(*m_smf);
                    } else {
                        event.status = prevStatus;
                    }
                    readMidiEvent(event);
                    prevStatus = event.status;
                }
            }
            std::cout << m_currentTrack->midiEvents.size() << std::endl;

        } else {
            std::cout << "Supported track chunk not found." << std::endl;
        }

        m_smf->seekg(pos);
    }

    if (pos == fileEnd) {
        std::cout << "End of MIDI reached." << std::endl;
    } else {
        std::cout << "Warning: Reached end of last track chunk, there is possibly data located outside of a track." << std::endl;
    }

}

SmfReader::SmfReader(std::string filename)
{
    m_smf = std::make_unique<std::ifstream>(filename, std::ios_base::ate  | std::ios_base::binary);

    if (*m_smf) {
        readFile();
        m_smf->close();
    } else {
        std::cout << "Failed to load MIDI file." << std::endl;
    }
}


std::vector<SmfTrack*> SmfReader::getTracks()
{

    std::vector<SmfTrack*> tracks;

    for (int i = 0; i < m_tracks.size(); i++) {
        tracks.push_back(m_tracks[i].get());
    }
    return tracks;
}
