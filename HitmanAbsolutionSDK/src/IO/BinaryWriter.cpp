#include "IO/BinaryWriter.h"

BinaryWriter::BinaryWriter()
{
	outputFileStream = nullptr;
	outputMemoryStream = nullptr;
}

BinaryWriter::BinaryWriter(const std::string& filePath)
{
	outputFileStream = new OutputFileStream(filePath);
	outputMemoryStream = nullptr;
}

BinaryWriter::BinaryWriter(const size_t dataSize)
{
	outputFileStream = nullptr;
	outputMemoryStream = new OutputMemoryStream(dataSize);
}

BinaryWriter::BinaryWriter(void* data, const size_t dataSize)
{
	outputFileStream = nullptr;
	outputMemoryStream = new OutputMemoryStream(data, dataSize);
}

BinaryWriter::~BinaryWriter()
{
	if (outputFileStream)
	{
		delete outputFileStream;
	}
	else
	{
		delete outputMemoryStream;
	}
}

void* BinaryWriter::GetBuffer() const
{
	if (outputFileStream)
	{
		return outputFileStream->GetBuffer();
	}

	return outputMemoryStream->GetBuffer();
}

size_t BinaryWriter::GetPosition()
{
	if (outputFileStream)
	{
		return outputFileStream->GetPosition();
	}

	return outputMemoryStream->GetPosition();
}

size_t BinaryWriter::GetCapacity() const
{
	if (outputFileStream)
	{
		return outputFileStream->GetCapacity();
	}

	return outputMemoryStream->GetCapacity();
}

void BinaryWriter::WriteString(const std::string& string)
{
	if (outputFileStream)
	{
		outputFileStream->WriteString(string);
	}
	else
	{
		outputMemoryStream->WriteString(string);
	}
}

void BinaryWriter::Skip(size_t count)
{
	if (outputFileStream)
	{
		outputFileStream->Skip(count);
	}
	else
	{
		outputMemoryStream->Skip(count);
	}
}

void BinaryWriter::Seek(size_t offset, SeekOrigin seekOrigin)
{
	if (outputFileStream)
	{
		outputFileStream->Seek(offset, seekOrigin);
	}
	else
	{
		outputMemoryStream->Seek(offset, seekOrigin);
	}
}

void BinaryWriter::AlignTo(const unsigned char alignment)
{
	if (outputFileStream)
	{
		outputFileStream->AlignTo(alignment);
	}
	else
	{
		outputMemoryStream->AlignTo(alignment);
	}
}
