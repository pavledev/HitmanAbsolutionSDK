#include "IO/OutputFileStream.h"

OutputFileStream::OutputFileStream(const std::string& filePath)
{
    outputFileStream = std::ofstream(filePath, std::ios::binary);
}

OutputFileStream::~OutputFileStream()
{
    if (outputFileStream.is_open())
    {
        outputFileStream.close();
    }
}

void* OutputFileStream::GetBuffer() const
{
    return nullptr;
}

size_t OutputFileStream::GetPosition()
{
    return static_cast<size_t>(outputFileStream.tellp());
}

size_t OutputFileStream::GetCapacity() const
{
    return 0;
}

void OutputFileStream::WriteString(const std::string& string)
{
    outputFileStream.write(string.data(), string.length() + 1);
}

void OutputFileStream::Skip(const size_t count)
{
    outputFileStream.seekp(count, std::ios_base::cur);
}

void OutputFileStream::Seek(const size_t offset, const SeekOrigin seekOrigin)
{
    outputFileStream.seekp(offset, static_cast<std::ios_base::seekdir>(seekOrigin));
}

void OutputFileStream::AlignTo(const unsigned char alignment)
{
}
