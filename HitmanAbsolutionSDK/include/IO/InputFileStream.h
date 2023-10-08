#pragma once

#include <fstream>

#include "SeekOrigin.h"
#include <Common.h>

class HitmanAbsolutionSDK_API InputFileStream
{
public:
	InputFileStream();
	InputFileStream(const std::string& filePath);
	~InputFileStream();
	const void* GetBuffer(bool seekToCurrentPosition = false) const;
	void* GetBuffer(bool seekToCurrentPosition = false);
	const size_t GetPosition();
	const size_t GetSize() const;

	template <typename T>
	T Read()
	{
		T data{};

		inputFileStream.read(reinterpret_cast<char*>(&data), sizeof(T));

		return data;
	}

	template <typename T>
	T* Read(const size_t size)
	{
		T* buffer = new T[size];

		inputFileStream.read(reinterpret_cast<char*>(buffer), size);

		return buffer;
	}

	template <>
	void* Read(const size_t size)
	{
		void* buffer = operator new(size);

		inputFileStream.read(reinterpret_cast<char*>(buffer), size);

		return buffer;
	}

	template<typename T>
	T* Read(const size_t size) const
	{
		T* buffer = new T[size];

		inputFileStream.read(reinterpret_cast<char*>(buffer), size);

		return buffer;
	}

	template <typename T>
	void Read(T* data, const size_t size)
	{
		inputFileStream.read(reinterpret_cast<char*>(data), sizeof(T) * size);
	}

	template <>
	void Read(void* data, const size_t size)
	{
		inputFileStream.read(reinterpret_cast<char*>(data), size);
	}

	std::string ReadString(const char delimiter = '\0');
	std::string ReadString(const size_t size, const bool isNullTerminated = true);

	void Skip(const size_t count);
	void Seek(const size_t offset, const SeekOrigin seekOrigin = SeekOrigin::Begin);

private:
	std::ifstream inputFileStream;
	size_t size;
};
