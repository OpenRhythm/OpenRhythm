#pragma once
#include <fstream>
#include <cstdint>

template<typename T>
T read_type(std::ifstream &file)
{
    T output;
    auto size = sizeof(T);
    int offset;
    for (int i = 0; i < size; i++) {
        offset = (size-1) - i; // endianness lol ?
        file.read(reinterpret_cast<char*>(&output)+offset, 1);
    }
    return output;
}

template<typename T>
void read_type(std::ifstream &file, T *output, unsigned long length)
{
    auto size = sizeof(T);
    int offset;
    for (int j = 0; j < length; j++) {
        for (int i = 0; i < size; i++) {
            offset = ((size-1) - i) + (size * j); // endianness lol ?
            file.read(reinterpret_cast<char*>(output) + offset, 1);
        }
    }
}

template<typename T>
T peek_type(std::ifstream &file)
{
    T output;
    auto size = sizeof(T);
    int offset;
    uint8_t *val;
    for (int i = 0; i < size; i++) {
        offset = (size-1) - i; // endianness lol ?
        val = reinterpret_cast<uint8_t*>(&output)+offset;
        *val = file.peek();
    }
    return output;
}

template<typename T>
void peek_type(std::ifstream &file, T *output, unsigned long length)
{
    auto size = sizeof(T);
    int offset;
    uint8_t *val;
    for (int j = 0; j < length; j++) {
        for (int i = 0; i < size; i++) {
            offset = ((size-1) - i) + (size * j); // endianness lol ?
            val = reinterpret_cast<uint8_t*>(&output)+offset;
            *val = file.peek();
        }
    }
}