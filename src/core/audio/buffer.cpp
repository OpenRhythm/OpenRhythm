// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "buffer.hpp"

namespace ORCore
{
    Buffer::Buffer()
    :m_allocated(false), m_info({0,0}), m_size(0)
    {
    }

    Buffer::Buffer(BufferInfo info)
    :m_allocated(true), m_info(info), m_size(info.channels * info.frames)
    {
        m_buffer = new float[m_size];
    }

    Buffer::Buffer(float* buffer, BufferInfo info)
    :m_allocated(false), m_info(info), m_size(info.channels * info.frames)
    {
        m_buffer = buffer;
    }

    Buffer::Buffer(const Buffer& other)
    :m_allocated(true), m_info(other.m_info), m_size(other.m_size)
    {
        m_buffer = new float[m_size];
        std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);
    }

    Buffer::Buffer(Buffer&& other)
    :m_allocated(other.m_allocated), m_info(other.m_info), m_size(other.m_size)
    {
        m_buffer = other.m_buffer;
        other.m_buffer = nullptr;
        other.m_allocated = false;
        other.m_info = {0, 0};
        other.m_size = 0;
    }

    Buffer::~Buffer()
    {
        if (m_allocated)
        {
            delete[] m_buffer;
        }
    }

    Buffer& Buffer::operator=(const Buffer& other)
    {
        if (this != &other)
        {
            if (m_size != other.m_size)
            {
                // If we previously allocated and the new buffer size is different
                // free the old memory
                if (m_allocated)
                {
                    delete[] m_buffer;
                }
                m_buffer = new float[m_size];
                m_allocated = true;

            }
            std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);

            m_info = other.m_info;
            m_size = other.m_size;
        }
        return *this;
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (this != &other)
        {
            m_buffer = other.m_buffer;
            m_allocated = other.m_allocated;
            m_info = other.m_info;
            m_size = other.m_size;

            // set old object to sane defaults
            other.m_buffer = nullptr;
            other.m_allocated = false;
            other.m_info = {0, 0};
            other.m_size = 0;
        }
        return *this;

    }

    Buffer::operator float*()
    {
        return m_buffer;
    }


    void Buffer::clone_type(const Buffer& other)
    {
        if (this != &other)
        {
            if (m_size != other.m_size)
            {
                // If we previously allocated and the new buffer size is different
                // free the old memory
                if (m_allocated)
                {
                    delete[] m_buffer;
                }
                m_buffer = new float[m_size];
                m_allocated = true;

            }
            m_info = other.m_info;
            m_size = other.m_size;
        }
    }

    BufferInfo Buffer::get_info()
    {
        return m_info;
    }

    int Buffer::size()
    {
        return m_size;
    }

    void Buffer::clear()
    {
        std::fill(m_buffer, m_buffer+m_size, 0.0f);
    }
}