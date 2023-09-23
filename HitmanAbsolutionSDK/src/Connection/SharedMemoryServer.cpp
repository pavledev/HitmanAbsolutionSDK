#include "Connection/SharedMemoryServer.h"
#include "Logger.h"

SharedMemoryServer::SharedMemoryServer()
{
	mapFile = nullptr;
	data = nullptr;
}

SharedMemoryServer& SharedMemoryServer::GetInstance()
{
	static SharedMemoryServer instance;

	return instance;
}

void SharedMemoryServer::Start()
{
	mapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(Data), "FileMappingObject");

	if (mapFile == INVALID_HANDLE_VALUE)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create file mapping object. Error: {}", Logger::GetLastError());

		return;
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Successfully created file mapping object.");
	}

	if (mapFile)
	{
		data = static_cast<Data*>(MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Data)));

		if (!data)
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Could not map view of file. Error: {}", Logger::GetLastError());

			CloseHandle(mapFile);
		}
	}
}

void SharedMemoryServer::Stop()
{
	UnmapViewOfFile(data);
	CloseHandle(mapFile);
}

SharedMemoryServer::Data* SharedMemoryServer::GetData()
{
	return data;
}
