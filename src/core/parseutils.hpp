#pragma once
#include <fstream>
#include <cstdint>
#include <stdexcept>

namespace ORCore
{
    template<typename T>
    T read_type(std::istream &file)
    {
        T output;
        auto size = sizeof(T);
        int offset;
        for (size_t i = 0; i < size; i++) {
            offset = (size-1) - i; // endianness lol ?
            file.read(reinterpret_cast<char*>(&output)+offset, 1);
        }
        return output;
    }

    template<typename T>
    T read_type(std::istream &file, size_t size)
    {
        T output = 0;
        if (sizeof(output) < size)
        {
            throw std::runtime_error("Size greater than container type");
        } else {
            int offset;
            for (size_t i = 0; i < size; i++) {
                offset = static_cast<int>((size-1) - i); // endianness lol ?
                file.read(reinterpret_cast<char*>(&output)+offset, 1);
            }
        }
        return output;
    }

    // Main purpose is for reading string-like data from the file.
    template<typename T>
    void read_type(std::istream &file, T *output, unsigned long length)
    {
        auto size = sizeof(T);
        int offset;
        for (size_t j = 0; j < length; j++) {
            for (size_t i = 0; i < size; i++) {
                offset = ((size-1) - i) + (size * j); // endianness lol ?
                file.read(reinterpret_cast<char*>(output) + offset, 1);
            }
        }
    }

    template<typename T>
    T peek_type(std::istream &file)
    {
        T output;
        auto size = sizeof(T);
        int offset;
        uint8_t *val;
        for (size_t i = 0; i < size; i++) {
            offset = (size-1) - i; // endianness lol ?
            val = reinterpret_cast<uint8_t*>(&output)+offset;
            *val = file.peek();
        }
        return output;
    }

    template<typename T>
    T peek_type(std::istream &file, size_t size)
    {
        T output = 0;
        if (sizeof(output) < size)
        {
            throw std::runtime_error("Size greater than container type");
        } else {
            int offset;
            uint8_t *val;
            for (size_t i = 0; i < size; i++) {
                offset = static_cast<int>((size-1) - i); // endianness lol ?
                val = reinterpret_cast<uint8_t*>(&output)+offset;
                *val = file.peek();
            }
        }
        return output;
    }

    // Main purpose is for reading string-like data from the file.
    template<typename T>
    void peek_type(std::istream &file, T *output, unsigned int length)
    {
        auto size = sizeof(T);
        unsigned int offset;
        uint8_t *val;
        for (size_t j = 0; j < length; j++) {
            for (size_t i = 0; i < size; i++) {
                offset = ((size-1) - i) + (size * j); // endianness lol ?
                val = reinterpret_cast<uint8_t*>(&output)+offset;
                *val = file.peek();
            }
        }
    }
}
