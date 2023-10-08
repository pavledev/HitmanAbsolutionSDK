#include <string>

#include "IO/InputFileStream.h"

InputFileStream::InputFileStream()
{
	size = 0;
}

InputFileStream::InputFileStream(const std::string& filePath)
{
	inputFileStream = std::ifstream(filePath, std::ios::binary | std::ios_base::ate);

	inputFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	size = static_cast<size_t>(inputFileStream.tellg());

	inputFileStream.seekg(0);
}

InputFileStream::~InputFileStream()
{
	if (inputFileStream.is_open())
	{
		inputFileStream.close();
	}
}

const void* InputFileStream::GetBuffer(bool seekToCurrentPosition) const
{
	return nullptr;
}

void* InputFileStream::GetBuffer(bool seekToCurrentPosition)
{
	return nullptr;
}

const size_t InputFileStream::GetPosition()
{
	return static_cast<size_t>(inputFileStream.tellg());
}

const size_t InputFileStream::GetSize() const
{
	return size;
}

std::string InputFileStream::ReadString(const char delimiter)
{
	std::string result;

	std::getline(inputFileStream, result, delimiter);

	return result;
}

std::string InputFileStream::ReadString(const size_t size, const bool isNullTerminated)
{
	size_t size2 = size;

	if (isNullTerminated)
	{
		++size2;
	}

	char* buffer = new char[size2];

	inputFileStream.read(buffer, size2);

	std::string result = std::string(buffer, size);

	delete[] buffer;

	return result;
}

void InputFileStream::Skip(const size_t count)
{
	inputFileStream.seekg(count, std::ios_base::cur);
}

void InputFileStream::Seek(const size_t offset, const SeekOrigin seekOrigin)
{
	inputFileStream.seekg(offset, static_cast<std::ios_base::seekdir>(seekOrigin));
}
