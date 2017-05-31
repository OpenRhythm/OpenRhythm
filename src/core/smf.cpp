#include "config.hpp"
#include "smf.hpp"
#include <iostream>

namespace ORCore
{

    SmfReader::SmfReader(std::string filename)
    :m_logger(spdlog::get("default"))
    {
        m_logger->info(_("Loading MIDI"));

        try
        {
            m_smfFile.load(filename);
        }
        catch (std::runtime_error &err)
        {
            throw std::runtime_error(_("Failed to load MIDI file."));
        }

        m_logger->info(_("Parsing midi."));

        read_file();
        m_smfFile.release();
    }

    std::vector<SmfTrack*> SmfReader::get_tracks()
    {
        std::vector<SmfTrack*> tracks;

        for (auto &track : m_tracks) {
            tracks.push_back(&track);
        }
        return tracks;
    }

    TempoTrack* SmfReader::get_tempo_track()
    {
        return &m_tempoTrack;
    }
    
    void SmfReader::release()
    {
        m_tracks.clear();
    }

    uint32_t SmfReader::read_var_len()
    {
        uint8_t c = read_type<uint8_t>(m_smfFile);
        uint32_t value = static_cast<uint32_t>(c & 0x7F);

        if (c & 0x80)
        {

            do
            {
                c = read_type<uint8_t>(m_smfFile);
                value = (value << 7) + (c & 0x7F);

            }
            while (c & 0x80);

        }
        return value;
    }

    void SmfReader::read_midi_event(const SmfEventInfo &event)
    {
        MidiEvent midiEvent;
        midiEvent.info = event;
        midiEvent.message = static_cast<MidiChannelMessage>(event.status & 0xF0);
        midiEvent.channel = static_cast<uint8_t>(event.status & 0xF);

        switch (midiEvent.message)
        {
            case NoteOff:      // note off           (2 more bytes)
            case NoteOn:       // note on            (2 more bytes)
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // note
                midiEvent.data2 = read_type<uint8_t>(m_smfFile); // velocity
                break;
            case KeyPressure:
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // note
                midiEvent.data2 = read_type<uint8_t>(m_smfFile); // pressure
                break;
            case ControlChange:
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // controller
                midiEvent.data2 = read_type<uint8_t>(m_smfFile); // cont_value
                break;
            case ProgramChange:
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // program
                midiEvent.data2 = 0; // no data
                break;
            case ChannelPressure:
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // pressure
                midiEvent.data2 = 0; // no data
                break;
            case PitchBend:
                midiEvent.data1 = read_type<uint8_t>(m_smfFile); // pitch_low
                midiEvent.data2 = read_type<uint8_t>(m_smfFile); // pitch_high
                break;
            default:
                m_logger->warn("Bad Midi control message {}", midiEvent.message);
        }

        m_currentTrack->midiEvents.push_back(midiEvent);
    }

    void SmfReader::read_meta_event(const SmfEventInfo &eventInfo)
    {
        MetaEvent event {eventInfo, read_type<MidiMetaEvent>(m_smfFile), read_var_len()};

        // In the cases where we dont implement an event type log it, and its data.
        switch(event.type)
        {
            case meta_SequenceNumber:
            {
                auto sequenceNumber = read_type<uint16_t>(m_smfFile);
                m_logger->trace(_("Sequence Number {}"), sequenceNumber);
                break;
            }
            case meta_Text:
            case meta_Copyright:
            case meta_InstrumentName:
            case meta_Lyrics: // TODO - Implement ruby parser for lyrics
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
                read_type<char>(m_smfFile, textData.get(), event.length);
                m_currentTrack->textEvents.push_back({event, std::string(textData.get())});
                break;
            }
            case meta_TrackName:
            {
                auto textData = std::make_unique<char[]>(event.length+1);
                textData[event.length] = '\0';

                read_type<char>(m_smfFile, textData.get(), event.length);
                m_currentTrack->name = std::string(textData.get());
                break;
            }
            case meta_MIDIChannelPrefix:
            {
                // TODO - Add channel 
                auto midiChannel = read_type<uint8_t>(m_smfFile);
                m_logger->trace(_("Midi Channel {}"), midiChannel);
                break;
            }
            case meta_EndOfTrack:
            {
                m_logger->trace(_("End of Track {} at time"), m_currentTrack->name);
                m_currentTrack->endTime = event.info.pulseTime;
                break;
            }
            case meta_Tempo:
            {
                double absTime;

                // We calculate the absTime of each tempo event from the previous which will act as a base to calculate other events time.
                // This is good because it reduces the number of doubles we store reducing memory usage somewhat, and it also reduces
                // the rounding error overall allowing more accurate timestamps. Thanks FireFox of the RGC discord for this idea from his
                // .chart/midi parser that is used for his moonscraper project.
                if (m_tempoTrack.tempo.size() == 0)
                {
                    absTime = 0.0;
                } else {
                    auto lastTempo = m_tempoTrack.tempo.back();
                    absTime = lastTempo.absTime + delta_tick_to_delta_time(&lastTempo, eventInfo.pulseTime - lastTempo.info.info.pulseTime );
                }

                uint32_t qnLength = read_type<uint32_t>(m_smfFile, 3);

                double timePerTick = (qnLength / (m_header.division * 1'000'000.0));

                m_tempoTrack.tempoOrdering.push_back({
                    TtOrderType::Tempo,
                    static_cast<int>(m_tempoTrack.tempo.size())
                });

                m_tempoTrack.tempo.push_back({event, qnLength, absTime, timePerTick});

                break;
            }
            case meta_TimeSignature:
            {
                TimeSignatureEvent tsEvent;
                tsEvent.info = event;
                tsEvent.numerator = read_type<uint8_t>(m_smfFile); // 4 default
                tsEvent.denominator = std::pow(2, read_type<uint8_t>(m_smfFile)); // 4 default

                // This is best described as a bad attempt at supporting meter and is basically useless.
                // The midi spec examples are also extremely misleading
                tsEvent.clocksPerBeat = read_type<uint8_t>(m_smfFile); // Standard is 24

                // The number of 1/32nd notes per "MIDI quarter note"
                // This should be used in order to change the note value which a "MIDI quarter note" is considered.
                // For example changing it to be 0x0C(12) should change tempo to be
                // defined in.
                // Note "MIDI quarter note" is defined to always be 24 midi clocks therfor even if
                // changed to dotted quarter it should still be 24 midi clocks
                //
                // Later thoughts, this could also mean to change globally what a quarter note means which basically
                // would make it totally unrelated to ts... But i still stand by my original interpritation as I have
                // found a reference that contains the same interpritation as above:
                //
                // Beyond MIDI: The Handbook of Musical Codes page 54 

                tsEvent.thirtySecondPQN = read_type<uint8_t>(m_smfFile); // 8 default

                m_logger->info(_("Time signature  {}/{} CPC: {} TSPQN: {}"),
                                    tsEvent.numerator,
                                    tsEvent.denominator,
                                    tsEvent.clocksPerBeat,
                                    tsEvent.thirtySecondPQN);

                m_tempoTrack.tempoOrdering.push_back({
                        TtOrderType::TimeSignature,
                        static_cast<int>(m_tempoTrack.timeSignature.size())
                    });

                m_tempoTrack.timeSignature.push_back(tsEvent);

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
                // store data for unused event for later save passthrough.
                m_logger->debug(_("Unused event type {}."), event.type);
                std::vector<char> eventData;
                for (int i=0;i<event.length;++i)
                {
                    eventData.emplace_back(read_type<char>(m_smfFile));
                }
                m_currentTrack->miscMeta.push_back({event, eventData});
                break;
            }
        }
    }

    void SmfReader::read_sysex_event(const SmfEventInfo &event)
    {
        auto length = read_var_len();
        std::vector<char> sysex;
        sysex.resize(length);
        read_type<char>(m_smfFile, &sysex[0], length);
        m_logger->trace(_("sysex event at position {}"), m_smfFile.get_pos());
    }

    // Convert from deltaPulses to deltaTime.
    double SmfReader::delta_tick_to_delta_time(TempoEvent* tempo, uint32_t deltaPulses)
    {
        return deltaPulses * tempo->timePerTick;
    }

    // Converts a absolute time in pulses to an absolute time in seconds
    double SmfReader::pulsetime_to_abstime(uint32_t pulseTime)
    {
        if (pulseTime == 0)
        {
            return 0.0;
        }
        // The basic idea here is to start from the most recent tempo event before this time.
        // Then calculate and add the time since that tempo to the tempo time.
        // To speed this up further we could store the result of the time per tick calculation in the tempo event and only use it here.
        TempoEvent* tempo = get_last_tempo_via_pulses(pulseTime);
        return tempo->absTime + ((pulseTime - tempo->info.info.pulseTime) * tempo->timePerTick);
    }

    void SmfReader::set_default_tempo_ts()
    {
        if (m_tempoTrack.timeSignature.size() == 0)
        {
            m_logger->info(_("Setting default time signature of 4/4."));

            MetaEvent tsEvent {{meta_TimeSignature,0,0}, meta_Tempo, 3};

            m_tempoTrack.tempoOrdering.push_back({
                TtOrderType::TimeSignature,
                static_cast<int>(m_tempoTrack.timeSignature.size())
            });

            m_tempoTrack.timeSignature.push_back({tsEvent, 4, 4, 24, 8});
        }

        if (m_tempoTrack.tempo.size() == 0)
        {
            m_logger->info(_("Setting default tempo of 120 BPM."));

            // We construct a new tempo event that will have a default
            // equivelent to 120 BPM the same thing will need to be done
            // for the time signature meta event.
            MetaEvent tempoEvent {{status_MetaEvent,0,0}, meta_Tempo, 3};

            m_tempoTrack.tempoOrdering.push_back({
                TtOrderType::Tempo,
                static_cast<int>(m_tempoTrack.tempo.size())
            });

            m_tempoTrack.tempo.push_back({tempoEvent, 500'000, 0.0}); // ppqn, absTime
        }
    }

    TempoEvent* SmfReader::get_last_tempo_via_pulses(uint32_t pulseTime)
    {
        static unsigned int value = 0;
        static uint32_t lastPulseTime = 0;

        std::vector<TempoEvent> &tempos = m_tempoTrack.tempo;

        if (pulseTime > 0)
        {
            set_default_tempo_ts();
        }

        // Ignore the cached last tempo value if the new pulse time is older.
        if (lastPulseTime > pulseTime)
        {
            value = 0;
        }

        for (unsigned int i = value; i < tempos.size(); i++)
        {
            if (tempos[i].info.info.pulseTime > pulseTime)
            {
                value = i-1;
                lastPulseTime = pulseTime;
                return &tempos[value];
            }
        }
        // return last value if nothing else is found
        return &tempos.back();  
    }

    SmfHeaderChunk* SmfReader::get_header()
    {
        return &m_header;
    }

    void SmfReader::read_events(uint32_t chunkEnd)
    {
        uint32_t pulseTime = 0;
        uint8_t oldRunningStatus = 0;
        bool runningStatusReset = false;

        // find a ballpark size estimate for the track 
        uint32_t sizeGuess = (chunkEnd - m_smfFile.get_pos()) / 3;
        m_currentTrack->midiEvents.reserve(sizeGuess);

        SmfEventInfo eventInfo;

        while (m_smfFile.get_pos() < chunkEnd)
        {

            eventInfo.deltaPulses = read_var_len();

            // DO NOT use this for time calculations.
            // You must convert each deltaPulse to a time
            // within the currently active tempo.
            pulseTime += eventInfo.deltaPulses;

            eventInfo.pulseTime = pulseTime;

            auto status = peek_type<uint8_t>(m_smfFile);

            if (status == status_MetaEvent)
            {
                if (runningStatusReset == false)
                {
                    runningStatusReset = true;
                    oldRunningStatus = eventInfo.status;
                    m_logger->trace("Old Running Status: {}", oldRunningStatus);
                }
                eventInfo.status = read_type<uint8_t>(m_smfFile);
                read_meta_event(eventInfo);
            }
            else if (status == status_SysexEvent || status == status_SysexEvent2)
            {
                if (runningStatusReset == false)
                {
                    runningStatusReset = true;
                    oldRunningStatus = eventInfo.status;
                    m_logger->trace("Old Running Status: {}", oldRunningStatus);
                }
                eventInfo.status = read_type<uint8_t>(m_smfFile);
                read_sysex_event(eventInfo);
            }
            else
            {
                // Check if we should use the running status.
                if ((status & 0xF0) >= 0x80)
                {
                    eventInfo.status = read_type<uint8_t>(m_smfFile);
                }
                else if (runningStatusReset)
                {
                    m_logger->warn("Running status after a reset. This is non-standard. Attempting to correct...");
                    m_logger->trace("Using status: {}", oldRunningStatus);
                    eventInfo.status = oldRunningStatus;
                }

                runningStatusReset = false;
                read_midi_event(eventInfo);
            }
        }
    }

    void SmfReader::read_file()
    {
        uint32_t fileEnd = m_smfFile.get_size();

        uint32_t fileStart = m_smfFile.get_pos();
        uint32_t filePos = fileStart;
        uint32_t fileRemaining = fileEnd;

        SmfChunkInfo chunk;

        // set the intial chunk starting position at the beginning of the file.
        uint32_t chunkStart = fileStart;

        // The chunk end will be calculated after a chunk is loaded.
        uint32_t chunkEnd = 0;

        int trackChunkCount = 0;

        // We could loop through the number of track chunks given in the header.
        // However if there are any unknown chunk types inside the midi file
        // this will likely break. So we just loop until we hit the end of the
        // file instead...
        while (filePos < fileEnd)
        {

            read_type<char>(m_smfFile, chunk.chunkType, 4);
            chunk.length = read_type<uint32_t>(m_smfFile);
            chunkEnd = chunkStart + (8 + chunk.length); // 8 is the length of the type + length fields

            m_logger->trace(_("chunk of type {} detected."), chunk.chunkType);
            // MThd chunk is only in the beginning of the file.
            if (chunkStart == fileStart && strcmp(chunk.chunkType, "MThd") == 0)
            {
                // Load header chunk
                m_header.info = chunk;
                m_header.format = read_type<uint16_t>(m_smfFile);
                m_header.trackNum = read_type<uint16_t>(m_smfFile);
                m_header.division = read_type<int16_t>(m_smfFile);

                // Make sure we reserve enough space for m_tracks just in-case.
                m_tracks.reserve(sizeof(SmfTrack) * m_header.trackNum);

                if (m_header.format == smfType0 && m_header.trackNum != 1)
                {
                    throw std::runtime_error(_("Not a valid type 0 midi."));
                }
                else if (m_header.format == smfType2)
                {
                    throw std::runtime_error(_("Type 2 midi not supported."));
                }

                if ((m_header.division & 0x8000) != 0)
                {
                    throw std::runtime_error(_("SMPTE time division not supported"));
                }
            }
            else if (strcmp(chunk.chunkType, "MTrk") == 0)
            {
                trackChunkCount += 1;
                m_tracks.emplace_back();
                m_currentTrack = &m_tracks.back();
                read_events(chunkEnd);

            }
            else
            {
                m_logger->warn(_("Non-standard chunk of type {} detected, skipping."), chunk.chunkType);
            }

            filePos = chunkEnd;
            chunkStart = filePos;

            // Make sure that we are in the correct location in the chunk
            // If not seek to the correct location and output an error in the log.
            if (static_cast<int>(m_smfFile.get_pos()) != filePos)
            {
                m_logger->warn(_("Offset for chunk '{}' incorrect, seeking to correct location."), chunk.chunkType);
                m_logger->warn(_("Offset difference. expected: '{}' actual: '{}'"), m_smfFile.get_pos(), filePos);
                m_smfFile.set_pos(filePos);
            }
            fileRemaining = (fileEnd-filePos);
            if (fileRemaining != 0 && fileRemaining <= 8)
            {
                m_logger->warn(_("Ignoring remaining bytes, to few left in midi for another track."));
                // Skip the rest of the file.
                break;
            }
        }

        if (trackChunkCount != m_header.trackNum)
        {
            m_logger->warn(_("Track chunk count does not match header."));
        }
        m_logger->info(_("End of MIDI reached."));

    }
} // namespace ORCore
