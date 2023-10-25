#pragma once

#include <string>

#include "SeekOrigin.h"

class OutputMemoryStream
{
public:
    OutputMemoryStream();
    OutputMemoryStream(const size_t capacity);
    OutputMemoryStream(void* buffer, size_t capacity);
    ~OutputMemoryStream();
    void* GetBuffer() const;
    size_t GetPosition();
    size_t GetCapacity() const;

    template <typename T>
    void Write(const T& data)
    {
        EnsureCapacity(sizeof(T));

        *reinterpret_cast<T*>(reinterpret_cast<char*>(this->buffer) + position) = data;

        position += sizeof(T);
    }

    template <typename T>
    void Write(T* buffer, const size_t size)
    {
        EnsureCapacity(size);

        memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->buffer) + position), buffer, size);

        position += size;
    }

    template <>
    void Write(void* buffer, const size_t size)
    {
        EnsureCapacity(size);

        memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->buffer) + position), buffer, size);

        position += size;
    }

    void WriteString(const std::string& string);
    void Skip(size_t count);
    void Seek(size_t offset, SeekOrigin seekOrigin = SeekOrigin::Begin);
    void AlignTo(const unsigned char alignment);

private:
    void EnsureCapacity(const size_t size);

    void* buffer;
    size_t capacity;
    size_t position;
    bool isExternalBuffer;
};
