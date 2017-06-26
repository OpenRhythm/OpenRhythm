// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <memory>
#include <atomic>
#include <algorithm>

#include "buffer.hpp"

namespace ORCore
{

    struct StreamFormat
    {
        int sampleRate;
        int channels;
    };

    // Stream defines the basic interface that is needed between pipeline stages.
    // Everything else added ontop of this will be primarally for setup and control purposes.
    class Stream
    {
    public:

        // This is how an audio consumer gets the data it needs.
        // Each pipline stage will call to the previous stage until it reaches a source.
        // One should not assume that a buffer is empty, as buffers can and will be re-used.
        // if you would like to clear the buffer before using you can do `buffer.clear()`.
        virtual void pull(Buffer& buffer) = 0;

        // For intermediate pipeline stages this will likely just return the value of the previous stage.
        // Unless that stage changes formats. 
        virtual StreamFormat get_format() = 0;

        // Some streams will always be `false` such as mixers.
        // When a stream is paused its pull should not be called, and the buffer should be zerod.
        virtual bool is_paused() = 0;
    };

    // InputStream is a stream with one more source streams.
    class InputStream: public Stream
    {
    public:
        // Adds input stream to stream. Not all streams support multiple sources.
        // Returns false if the available inputs are already used.
        virtual bool add_source(Stream* stream) = 0;
    };

    // Producers do not accept inputs, can be paused and are required to keep track of time info.
    // This is really just mainly a convenience thing.
    class ProducerStream: public Stream
    {
    public:

        double get_time()
        {
            return m_time.load(std::memory_order_acquire);
        }

        void set_time(double time)
        {
            m_time.store(time, std::memory_order_release);
        }

        // Thread safe pausing.
        bool is_paused()
        {
            return m_paused.load(std::memory_order_acquire);
        }

        void set_pause(bool pause)
        {
            m_paused.store(pause, std::memory_order_release);
        }

    protected:
        std::atomic_bool m_paused;
        std::atomic<double> m_time;
    };

    // A consumer is not a stream, it is the endpoint.
    // Some examples of a consumer is an output device or a network output stream.
    class Consumer
    {
    public:
        // A consumer can only accept a single audio stream.
        virtual void set_source(Stream* stream) = 0;

        // Setup output and start pulling data from input stream.
        virtual bool start() = 0;

        // Stop pulling data from input stream.
        virtual void stop() = 0;

    };



}