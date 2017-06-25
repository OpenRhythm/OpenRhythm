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

    // TODO - Actually move away from unique_ptr here to raw new/delete if needed because we need
    // to be able to pass in direct device pointers and such which would be bad if we tried to
    // free them. new/delete will only be called when the constructor with only size is passed in.
    // TODO - put channel # and number of frames in here.
    class Buffer
    {
    public:
        Buffer(BufferInfo info);
        Buffer(float* buffer, BufferInfo info);
        Buffer(const Buffer& other);
        Buffer(Buffer&& other);
        ~Buffer();

        Buffer& operator=(const Buffer& other);
        Buffer& operator=(Buffer&& other);

        operator float*();

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