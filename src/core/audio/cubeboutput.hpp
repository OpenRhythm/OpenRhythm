// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include "streams.hpp"

#include <spdlog/spdlog.h>
#include <cubeb/cubeb.h>

namespace ORCore
{
    class CubebOutput: public Consumer
    {
    public:
        CubebOutput();
        ~CubebOutput();
        bool start();
        void stop();
        int process(float* buffer, int frameCount);
        void set_source(Stream* stream);

    private:
        std::shared_ptr<spdlog::logger> m_logger;
        Stream* m_source = nullptr;
        cubeb* m_context;
        cubeb_stream* m_stream;
    };
}