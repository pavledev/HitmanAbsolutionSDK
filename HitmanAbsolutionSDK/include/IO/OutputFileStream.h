#pragma once

#include <fstream>

#include "SeekOrigin.h"

class OutputFileStream
{
public:
    OutputFileStream(const std::string& filePath);
    ~OutputFileStream();
    void* GetBuffer() const;
    size_t GetPosition();
    size_t GetCapacity() const;

    template <typename T>
    void Write(const T& data)
    {
        outputFileStream.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    template <typename T>
    void Write(T* buffer, const size_t size)
    {
        outputFileStream.write(reinterpret_cast<const char*>(buffer), size);
    }

    template <>
    void Write(void* buffer, const size_t size)
    {
        outputFileStream.write(reinterpret_cast<const char*>(buffer), size);
    }

    void WriteString(const std::string& string);
    void Skip(const size_t count);
    void Seek(const size_t offset, const SeekOrigin seekOrigin = SeekOrigin::Begin);
    void AlignTo(const unsigned char alignment);

private:
    std::ofstream outputFileStream;
};
