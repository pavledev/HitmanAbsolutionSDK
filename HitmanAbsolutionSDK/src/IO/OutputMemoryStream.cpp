#include "IO/OutputMemoryStream.h"

OutputMemoryStream::OutputMemoryStream() : buffer(nullptr), capacity(0), position(0), isExternalBuffer(false)
{
}

OutputMemoryStream::OutputMemoryStream(size_t capacity) : capacity(capacity), position(0), isExternalBuffer(false)
{
    buffer = calloc(1, capacity);
}

OutputMemoryStream::OutputMemoryStream(void* buffer, size_t capacity) : buffer(buffer), capacity(capacity), position(0), isExternalBuffer(true)
{
}

OutputMemoryStream::~OutputMemoryStream()
{
    if (!isExternalBuffer)
    {
        delete[] buffer;
    }
}

void* OutputMemoryStream::GetBuffer() const
{
    return buffer;
}

size_t OutputMemoryStream::GetPosition()
{
    return position;
}

size_t OutputMemoryStream::GetCapacity() const
{
    return capacity;
}

void OutputMemoryStream::WriteString(const std::string& string)
{
    EnsureCapacity(string.length());

    Write(string.data(), string.length() + 1);
}

void OutputMemoryStream::Skip(size_t count)
{
    position += count;
}

void OutputMemoryStream::Seek(size_t offset, SeekOrigin seekOrigin)
{
    switch (seekOrigin)
    {
        case SeekOrigin::Begin:
        {
            position = offset;

            break;
        }
        case SeekOrigin::Current:
        {
            position += offset;

            break;
        }
        case SeekOrigin::End:
        {
            position = capacity - offset;

            break;
        }
    }
}

void OutputMemoryStream::EnsureCapacity(const size_t size)
{
    if (position + size > capacity)
    {
        size_t newCapacity = capacity * 2;

        while (newCapacity < size)
        {
            newCapacity *= 2;
        }

        buffer = realloc(buffer, newCapacity);

        size_t size2 = newCapacity - capacity;
        void* buffer2 = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + capacity);

        memset(buffer2, 0, size2);

        capacity = newCapacity;
    }
}

void OutputMemoryStream::AlignTo(const unsigned char alignment)
{
    if (position % alignment != 0)
    {
        const unsigned int numberOfBytesToSkip = alignment - position % alignment;

        EnsureCapacity(position + numberOfBytesToSkip);

        memset(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->buffer) + position), 0, numberOfBytesToSkip);

        position += numberOfBytesToSkip;
    }
}
