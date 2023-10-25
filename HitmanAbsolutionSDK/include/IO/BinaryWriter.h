#pragma once

#include "OutputFileStream.h"
#include "OutputMemoryStream.h"

class BinaryWriter
{
public:
    BinaryWriter();
    BinaryWriter(const std::string& filePath);
    BinaryWriter(const size_t dataSize);
    BinaryWriter(void* buffer, const size_t dataSize);
    ~BinaryWriter();
    void* GetBuffer() const;
    size_t GetPosition();
    size_t GetCapacity() const;

    template <typename T>
    void Write(const T& buffer)
    {
        if (outputFileStream)
        {
            outputFileStream->Write<T>(buffer);
        }
        else
        {
            outputMemoryStream->Write<T>(buffer);
        }
    }

    template <typename T>
    void Write(T* buffer, const size_t size)
    {
        if (outputFileStream)
        {
            outputFileStream->Write<T>(buffer, size);
        }
        else
        {
            outputMemoryStream->Write<T>(buffer, size);
        }
    }

    void WriteString(const std::string& string);
    void Skip(size_t count);
    void Seek(size_t offset, SeekOrigin seekOrigin = SeekOrigin::Begin);
    void AlignTo(const unsigned char alignment);

private:
    OutputFileStream* outputFileStream;
    OutputMemoryStream* outputMemoryStream;
};
