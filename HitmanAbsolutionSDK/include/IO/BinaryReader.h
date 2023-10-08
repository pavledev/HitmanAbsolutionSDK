#pragma once

#include <string>

#include "InputFileStream.h"
#include "InputMemoryStream.h"

class HitmanAbsolutionSDK_API BinaryReader
{
public:
	BinaryReader();
	BinaryReader(const std::string& filePath);
	BinaryReader(void* data, const size_t dataSize);
	~BinaryReader();
	const void* GetBuffer(bool seekToCurrentPosition = false) const;
	void* GetBuffer(bool seekToCurrentPosition = false);
	const size_t GetPosition();
	const size_t GetSize() const;

	template <typename T>
	T Read()
	{
		if (inputFileStream)
		{
			return inputFileStream->Read<T>();
		}

		return inputMemoryStream->Read<T>();
	}

	std::string ReadString(const char delimiter = '\0');
	std::string ReadString(const size_t size, const bool isNullTerminated = true);

	template <typename T>
	T* Read(const size_t size)
	{
		if (inputFileStream)
		{
			return inputFileStream->Read<T>(size);
		}

		return inputMemoryStream->Read<T>(size);
	}

	template <typename T>
	T* Read(const size_t size) const
	{
		if (inputFileStream)
		{
			return inputFileStream->Read<T>(size);
		}

		return inputMemoryStream->Read<T>(size);
	}

	template <typename T>
	void Read(T* buffer, const size_t size)
	{
		if (inputFileStream)
		{
			inputFileStream->Read<T>(buffer, size);
		}
		else
		{
			inputMemoryStream->Read<T>(buffer, size);
		}
	}

	template <>
	void Read(void* buffer, const size_t size)
	{
		if (inputFileStream)
		{
			inputFileStream->Read<void>(buffer, size);
		}
		else
		{
			inputMemoryStream->Read<void>(buffer, size);
		}
	}

	void Skip(const size_t count);
	void Seek(const size_t offset, const SeekOrigin seekOrigin = SeekOrigin::Begin);

private:
	InputFileStream* inputFileStream;
	InputMemoryStream* inputMemoryStream;
};
