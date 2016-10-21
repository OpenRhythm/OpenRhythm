#pragma once
#include <fstream>
#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>

#include <libintl.h>
#define _(STRING) gettext(STRING)

namespace ORCore
{
    // TODO - Could be useful to move into the VFS at some point.
    // Also in the future we can have an alternative memory mapped file 
    // implementation when speed is less important than memory usage.
    // This is just mainly a basic starting point.
    struct FileBuffer
    {
        std::unique_ptr<char[]> data;
        uint32_t position = 0;
        uint32_t size = 0;

        void load(std::string filename)
        {
            std::ifstream dataFile(filename, std::ios_base::ate | std::ios_base::binary);

            if (dataFile) {
                size = dataFile.tellg();
                data = std::make_unique<char[]>(size);
                dataFile.seekg(0, std::ios::beg);

                dataFile.read(&data[0], size);
                dataFile.close();
            } else {
                throw std::runtime_error(_("Failed to load file."));
            }
        }
        uint32_t get_pos() {
            return position;
        }
        void set_pos(uint32_t pos) {
            position = pos;
        }
        void set_pos_rel(uint32_t pos) {
            position += pos;
        }
        uint32_t get_size() {
            return size;
        }
    };

    // Custom FileBuffer based reading.
    template<typename T>
    T read_type(FileBuffer &fileData)
    {
        T output;
        size_t size = sizeof(T);

        char *outPtr = reinterpret_cast<char*>(&output);
        char *inPtr = &fileData.data[fileData.position];

        for(size_t i = 0; i < size; i++) {
            outPtr[i] = inPtr[size-1 - i];
        }
        fileData.position += size;
        return output;
    }

    template<typename T>
    T read_type(FileBuffer &fileData, size_t size)
    {
        T output = 0;
        if (sizeof(output) < size)
        {
            throw std::runtime_error(_("Size greater than container type"));
        } else {
            char *outPtr = reinterpret_cast<char*>(&output);
            char *inPtr = &fileData.data[fileData.position];

            for(size_t i = 0; i < size; i++) {
                outPtr[i] = inPtr[size-1 - i];
            }
            fileData.position += size;
        }
        return output;
    }

    // Main purpose is for reading string-like data from the file.
    template<typename T>
    void read_type(FileBuffer &fileData, T *output, unsigned long length)
    {
        size_t size = sizeof(T);

        char *outPtr = reinterpret_cast<char*>(output);
        char *inPtr;

        for (size_t j = 0; j < length; j++) {
            inPtr = &fileData.data[fileData.position];
            for(size_t i = 0; i < size; i++) {
                outPtr[i] = inPtr[size-1 - i];
            }
            outPtr += size;
            fileData.position += size;
        }
    }

    template<typename T>
    T peek_type(FileBuffer &fileData)
    {
        T output;
        size_t size = sizeof(T);

        char *outPtr = reinterpret_cast<char*>(&output);
        char *inPtr = &fileData.data[fileData.position];

        for(size_t i = 0; i < size; i++) {
            outPtr[i] = inPtr[size-1 - i];
        }
        return output;
    }

    template<typename T>
    T peek_type(FileBuffer &fileData, size_t size)
    {
        T output = 0;
        if (sizeof(output) < size)
        {
            throw std::runtime_error(_("Size greater than container type"));
        } else {
            char *outPtr = reinterpret_cast<char*>(&output);
            char *inPtr = &fileData.data[fileData.position];

            for(size_t i = 0; i < size; i++) {
                outPtr[i] = inPtr[size-1 - i];
            }
        }
        return output;
    }

    // Main purpose is for reading string-like data from the file.
    template<typename T>
    void peek_type(FileBuffer &fileData, T *output, unsigned long length)
    {
        size_t size = sizeof(T);

        char *outPtr = reinterpret_cast<char*>(output);
        char *inPtr;

        for (size_t j = 0; j < length; j++) {
            inPtr = &fileData.data[fileData.position+(size*j)];
            for(size_t i = 0; i < size; i++) {
                outPtr[i] = inPtr[size-1 - i];
            }
            outPtr += size;
        }
    }
}
