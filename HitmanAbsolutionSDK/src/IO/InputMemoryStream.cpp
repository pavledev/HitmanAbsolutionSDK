#include "IO/InputMemoryStream.h"

InputMemoryStream::InputMemoryStream() : buffer(nullptr), size(0), position(0)
{
}

InputMemoryStream::InputMemoryStream(void* buffer, const size_t dataSize) : buffer(buffer), size(dataSize), position(0)
{
}

const void* InputMemoryStream::GetBuffer(bool seekToCurrentPosition) const
{
	if (seekToCurrentPosition)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + position);
	}

	return buffer;
}

void* InputMemoryStream::GetBuffer(bool seekToCurrentPosition)
{
	if (seekToCurrentPosition)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + position);
	}

	return buffer;
}

const size_t InputMemoryStream::GetPosition()
{
	return position;
}

const size_t InputMemoryStream::GetSize() const
{
	return size;
}

std::string InputMemoryStream::ReadString(const char delimiter)
{
	char c;
	std::string result;

	while ((c = Read<char>()) != delimiter)
	{
		result += c;
	}

	return result;
}

std::string InputMemoryStream::ReadString(const size_t size, const bool isNullTerminated)
{
	size_t size2 = size;

	if (isNullTerminated)
	{
		++size2;
	}

	char* buffer = new char[size2];

	memcpy(buffer, reinterpret_cast<const char*>(this->buffer) + position, size2);

	position += size2;

	std::string result = std::string(buffer, size);

	delete[] buffer;

	return result;
}

void InputMemoryStream::Skip(const size_t count)
{
	position += count;
}

void InputMemoryStream::Seek(const size_t offset, const SeekOrigin seekOrigin)
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
			position = size - offset;

			break;
		}
	}
}

void InputMemoryStream::AlignTo(const unsigned char alignment)
{
}
