#pragma once

#include <string>

#include "SeekOrigin.h"
#include <Common.h>

class HitmanAbsolutionSDK_API InputMemoryStream
{
public:
	InputMemoryStream();
	InputMemoryStream(void* buffer, const size_t dataSize);
	const void* GetBuffer(bool seekToCurrentPosition = false) const;
	void* GetBuffer(bool seekToCurrentPosition = false);
	const size_t GetPosition();
	const size_t GetSize() const;

	template <typename T>
	T Read()
	{
		T data{};

		data = *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->buffer) + position);
		position += sizeof(T);

		return data;
	}

	template <typename T>
	T* Read(const size_t size)
	{
		T* buffer = new T[size];

		memcpy(buffer, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->buffer) + position), sizeof(T) * size);

		position += size;

		return buffer;
	}

	template <>
	void* Read(const size_t size)
	{
		void* buffer = operator new(size);

		memcpy(buffer, reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(this->buffer) + position), size);

		position += size;

		return buffer;
	}

	template<typename T>
	T* Read(const size_t size) const
	{
		T* buffer = new T[size];

		memcpy(buffer, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->buffer) + position), sizeof(T) * size);

		position += size;

		return buffer;
	}

	template <typename T>
	void Read(T* data, const size_t size)
	{
		memcpy(data, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(buffer) + position), sizeof(T) * size);

		position += size;
	}

	template <>
	void Read(void* data, const size_t size)
	{
		memcpy(data, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + position), size);

		position += size;
	}

	std::string ReadString(const char delimiter = '\0');
	std::string ReadString(const size_t size, const bool isNullTerminated = true);

	void Skip(const size_t count);
	void Seek(const size_t offset, const SeekOrigin seekOrigin = SeekOrigin::Begin);
	void AlignTo(const unsigned char alignment);

private:
	void* buffer;
	size_t size;
	size_t position;
};
