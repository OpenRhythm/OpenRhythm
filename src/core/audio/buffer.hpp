// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <memory>
#include <algorithm>

namespace ORCore
{
    struct BufferInfo
    {
        int channels;
        int frames;
    };

    class Buffer
    {
    public:
        Buffer();
        Buffer(BufferInfo info);
        Buffer(float* buffer, BufferInfo info);
        Buffer(const Buffer& other);
        Buffer(Buffer&& other);
        ~Buffer();

        Buffer& operator=(const Buffer& other);
        Buffer& operator=(Buffer&& other);

        operator float*();

        void clone_type(const Buffer& other);

        BufferInfo get_info();
        int size();
        void clear();

    private:
        float* m_buffer;
        bool m_allocated;
        BufferInfo m_info;
        int m_size;
    };
}