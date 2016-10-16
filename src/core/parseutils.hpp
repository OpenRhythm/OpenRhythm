#pragma once
#include <fstream>
#include <cstdint>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

namespace ORCore
{
    // This is 
    struct FileBuffer
    {
        std::vector<char> data;
        uint32_t position = 0;
        uint32_t size = 0;

        void load(std::string filename)
        {
            std::ifstream dataFile(filename, std::ios_base::ate | std::ios_base::binary);

            if (dataFile) {
                size = dataFile.tellg();
                data.resize(size);
                dataFile.seekg(0, std::ios::beg);

                dataFile.read(&data[0], size);
                dataFile.close();
            } else {
                throw std::runtime_error("Failed to load file.");
            }
        }
        size_t get_pos() {
            return position;
        }
        void set_pos(uint32_t pos) {
            position = pos;
        }
        void set_pos_rel(uint32_t pos) {
            position += pos;
        }
        size_t get_size() {
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
            throw std::runtime_error("Size greater than container type");
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
            throw std::runtime_error("Size greater than container type");
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


    // stream based reading
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
