#include "config.hpp"
#include "smf.hpp"

namespace ORCore
{
    uint32_t SmfReader::read_var_len()
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

    void SmfReader::read_midi_event(SmfEventInfo &event)
    {
        MidiEvent midiEvent;
        midiEvent.info = event;
        midiEvent.message = static_cast<MidiChannelMessage>(event.status & 0xF0);
        midiEvent.channel = static_cast<uint8_t>(event.status & 0xF);

        switch (midiEvent.message) {
            case NoteOff:      // note off           (2 more bytes)
            case NoteOn:       // note on            (2 more bytes)
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // note
                midiEvent.data2 = ORCore::read_type<uint8_t>(m_smfFile); // velocity
                break;
            case Aftertouch:
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // note
                midiEvent.data2 = ORCore::read_type<uint8_t>(m_smfFile); // pressure
                break;
            case ControlChange:
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // controller
                midiEvent.data2 = ORCore::read_type<uint8_t>(m_smfFile); // cont_value
                break;
            case ProgramChange:
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // program
                midiEvent.data2 = 0; // no data
                break;
            case ChannelPressure:
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // pressure
                midiEvent.data2 = 0; // no data
                break;
            case PitchWheel:
                midiEvent.data1 = ORCore::read_type<uint8_t>(m_smfFile); // pitch_low
                midiEvent.data2 = ORCore::read_type<uint8_t>(m_smfFile); // pitch_high
                break;
            default:
                m_logger->warn(_("Bad Midi control message"));
        }

        m_currentTrack->midiEvents.push_back(midiEvent);
    }

    void SmfReader::read_meta_event(SmfEventInfo &eventInfo)
    {
        MetaEvent event {eventInfo, ORCore::read_type<MidiMetaEvent>(m_smfFile), read_var_len()};

        // In the cases where we dont implement an event type log it, and its data.
        switch(event.type)
        {
            case meta_SequenceNumber:
            {
                auto sequenceNumber = ORCore::read_type<uint16_t>(m_smfFile);
                m_logger->trace(_("Sequence Number {}"), sequenceNumber);
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
                auto textData = std::make_unique<char[]>(event.length+1);
                textData[event.length] = '\0';
                ORCore::read_type<char>(m_smfFile, textData.get(), event.length);
                m_currentTrack->textEvents.push_back({event, std::string(textData.get())});
                break;
            }
            case meta_TrackName:
            {
                auto textData = std::make_unique<char[]>(event.length+1);
                textData[event.length] = '\0';

                ORCore::read_type<char>(m_smfFile, textData.get(), event.length);
                m_currentTrack->name = std::string(textData.get());
                break;
            }
            case meta_MIDIChannelPrefix: {
                auto midiChannel = ORCore::read_type<uint8_t>(m_smfFile);
                m_logger->trace(_("Midi Channel {}"), midiChannel);
                break;
            }
            case meta_EndOfTrack:
            {
                // TODO - We might be able to use this for some purpose.
                // Actually yeah, this will be needed for proper bpm marking.
                // That way we can mark bpm until the end of a track
                m_logger->trace(_("End of Track {}"), m_currentTrack->name);
                m_currentTrack->miscMeta.push_back({event, std::vector<char>()});
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

                m_logger->trace(_("Time signature  {}/{} CPC: {} TSPQN: {}"),
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
                m_logger->info(_("Unused event type {}."), event.type);
                m_smfFile.set_pos_rel(event.length);
                break;
            }
        }
    }

    void SmfReader::read_sysex_event(SmfEventInfo &event)
    {
        auto length = read_var_len();
        std::vector<char> sysex;
        sysex.resize(length);
        read_type<char>(m_smfFile, &sysex[0], length);
        m_logger->info(_("sysex even at position {}"), m_smfFile.get_pos());
    }

    double SmfReader::conv_abstime(uint32_t deltaPulses)
    {
        return deltaPulses * (m_currentTempoEvent->qnLength / (m_header.division * 1000000.0));
    }

    void SmfReader::read_events(uint32_t chunkEnd)
    {
        uint32_t pulseTime = 0;
        uint8_t prevStatus = 0;
        double currentRunningTimeSec = 0;

        while (m_smfFile.get_pos() < chunkEnd)
        {
            SmfEventInfo eventInfo;

            eventInfo.deltaPulses = read_var_len();

            // DO NOT use this for time calculations.
            // You must convert each deltaPulse to a time
            // within the currently active tempo.
            pulseTime += eventInfo.deltaPulses;

            eventInfo.pulseTime = pulseTime;

            if (pulseTime == 0) {
                eventInfo.absTime = 0.0;
            } else {
                currentRunningTimeSec += conv_abstime(eventInfo.deltaPulses);
                eventInfo.absTime = currentRunningTimeSec;
            }
            auto status = ORCore::peek_type<uint8_t>(m_smfFile);

            if (status == status_MetaEvent) {
                prevStatus = 0; // reset running status
                eventInfo.status = ORCore::read_type<uint8_t>(m_smfFile);
                read_meta_event(eventInfo);
            } else if (status == status_SysexEvent || status == status_SysexEvent2) {
                prevStatus = 0;  // reset running status
                eventInfo.status = ORCore::read_type<uint8_t>(m_smfFile);
                read_sysex_event(eventInfo);
            } else {
                if ((status & 0xF0) >= 0x80) {
                    eventInfo.status = ORCore::read_type<uint8_t>(m_smfFile);
                } else {
                    eventInfo.status = prevStatus;
                }
                read_midi_event(eventInfo);
                prevStatus = eventInfo.status;
            }

            if (pulseTime != 0 && (m_tempoTrack == nullptr || m_tempoTrack->tempo.size() == 0)) {

                m_logger->info(_("No tempo change at deltatime 0 setting default of 120 BPM."));

                // We construct a new tempo event that will have a default
                // equivelent to 120 BPM the same thing will need to be done
                // for the time signature meta event.
                MetaEvent tempoEvent {{status_MetaEvent,0,0,0.0}, meta_Tempo, 3};

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

                m_logger->info(_("No time signature change at deltatime 0 setting default of 4/4."));

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
                m_currentTempoEvent = get_last_tempo_via_pulses(pulseTime);
            }
        }
        m_currentTrack->seconds = static_cast<float>(currentRunningTimeSec);
    }

    void SmfReader::read_file()
    {
        int fileEnd = static_cast<int>(m_smfFile.get_size());

        int fileStart = static_cast<int>(m_smfFile.get_pos());
        int filePos = fileStart;
        int fileRemaining = fileEnd;

        SmfChunkInfo chunk;

        // set the intial chunk starting position at the beginning of the file.
        int chunkStart = fileStart;

        // The chunk end will be calculated after a chunk is loaded.
        uint32_t chunkEnd = 0;

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

            m_logger->trace(_("chunk of type {} detected."), chunk.chunkType);
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
                    throw std::runtime_error(_("Not a valid type 0 midi."));
                }

                // TODO - For completionist reasons eventually add support for this.
                if ((m_header.division & 0x8000) != 0) {
                    throw std::runtime_error(_("SMPTE time division not supported"));
                }

            } else if (strcmp(chunk.chunkType, "MTrk") == 0) {
                trackChunkCount += 1;
                m_tracks.emplace_back();
                m_currentTrack = &m_tracks.back();
                read_events(chunkEnd);

            } else {
                m_logger->warn(_("Non-standard chunk of type {} detected, skipping."), chunk.chunkType);
            }

            filePos = chunkEnd;
            chunkStart = filePos;

            // Make sure that we are in the correct location in the chunk
            // If not seek to the correct location and output an error in the log.
            if (static_cast<int>(m_smfFile.get_pos()) != filePos) {
                m_logger->warn(_("Offset for chunk '{}' incorrect, seeking to correct location."), chunk.chunkType);
                m_logger->warn(_("Offset difference. expected: '{}' actual: '{}'"), m_smfFile.get_pos(), filePos);
                m_smfFile.set_pos(filePos);
            }
            fileRemaining = (fileEnd-filePos);
            if (fileRemaining != 0 && fileRemaining <= 8) {
                m_logger->warn(_("To few bytes remaining in midi for another track, this is likely a bug."));
            }
        }
        if (trackChunkCount != m_header.trackNum) {
            m_logger->warn(_("Track chunk count does not match header."));
        }
        m_logger->info(_("End of MIDI reached."));

    }

    SmfReader::SmfReader(std::string filename)
    :m_tempoTrack(nullptr), m_timeSigTrack(nullptr), m_logger(spdlog::get("default"))
    {
        m_logger->info(_("Loading MIDI"));

        try {
            m_smfFile.load(filename);
        } catch (std::runtime_error &err) {
            throw std::runtime_error(_("Failed to load MIDI file."));
        }

        read_file();
    }

    TempoEvent* SmfReader::get_last_tempo_via_pulses(uint32_t pulseTime)
    {
        static unsigned int value = 0;
        static uint32_t lastPulseTime = 0;

        std::vector<TempoEvent> &tempos = m_tempoTrack->tempo;

        // Ignore the cached last tempo value if the new pulse time is older.
        if (lastPulseTime > pulseTime) {
            value = 0;
        }

        for (unsigned int i = value; i < tempos.size(); i++) {
            if (tempos[i].info.info.pulseTime >= pulseTime) {
                value = i;
                lastPulseTime = pulseTime;
                return &tempos[i];
            }
        }
        // return last value if nothing else is found
        return &tempos.back();
    }

    std::vector<SmfTrack*> SmfReader::get_tracks()
    {
        std::vector<SmfTrack*> tracks;

        for (auto &track : m_tracks) {
            tracks.push_back(&track);
        }
        return tracks;
    }
    SmfTrack* SmfReader::get_tempo_track()
    {
        if (m_header.format == smfType2) {
            return nullptr;
        }
        return m_timeSigTrack;
    }
    SmfTrack* SmfReader::get_time_sig_track()
    {
        if (m_header.format == smfType2) {
            return nullptr;
        }
        return m_timeSigTrack;
    }
} // namespace ORCore
