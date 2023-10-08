#include "IO/BinaryReader.h"

BinaryReader::BinaryReader()
{
	inputFileStream = nullptr;
	inputMemoryStream = nullptr;
}

BinaryReader::BinaryReader(const std::string& filePath)
{
	inputFileStream = new InputFileStream(filePath);
	inputMemoryStream = nullptr;
}

BinaryReader::BinaryReader(void* data, const size_t dataSize)
{
	inputFileStream = nullptr;
	inputMemoryStream = new InputMemoryStream(data, dataSize);
}

BinaryReader::~BinaryReader()
{
	if (inputFileStream)
	{
		delete inputFileStream;
	}
	else
	{
		delete inputMemoryStream;
	}
}

const void* BinaryReader::GetBuffer(bool seekToCurrentPosition) const
{
	if (inputFileStream)
	{
		return inputFileStream->GetBuffer(seekToCurrentPosition);
	}

	return inputMemoryStream->GetBuffer(seekToCurrentPosition);
}

void* BinaryReader::GetBuffer(bool seekToCurrentPosition)
{
	if (inputFileStream)
	{
		return inputFileStream->GetBuffer(seekToCurrentPosition);
	}

	return inputMemoryStream->GetBuffer(seekToCurrentPosition);
}

const size_t BinaryReader::GetPosition()
{
	if (inputFileStream)
	{
		return inputFileStream->GetPosition();
	}

	return inputMemoryStream->GetPosition();
}

const size_t BinaryReader::GetSize() const
{
	if (inputFileStream)
	{
		return inputFileStream->GetSize();
	}

	return inputMemoryStream->GetSize();
}

std::string BinaryReader::ReadString(const char delimiter)
{
	if (inputFileStream)
	{
		return inputFileStream->ReadString(delimiter);
	}

	return inputMemoryStream->ReadString(delimiter);
}

std::string BinaryReader::ReadString(const size_t size, const bool isNullTerminated)
{
	if (inputFileStream)
	{
		return inputFileStream->ReadString(size, isNullTerminated);
	}

	return inputMemoryStream->ReadString(size, isNullTerminated);
}

void BinaryReader::Skip(const size_t count)
{
	if (inputFileStream)
	{
		inputFileStream->Skip(count);
	}
	else
	{
		inputMemoryStream->Skip(count);
	}
}

void BinaryReader::Seek(const size_t offset, const SeekOrigin seekOrigin)
{
	if (inputFileStream)
	{
		inputFileStream->Seek(offset, seekOrigin);
	}
	else
	{
		inputMemoryStream->Seek(offset, seekOrigin);
	}
}
