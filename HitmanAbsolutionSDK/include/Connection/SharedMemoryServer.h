#pragma once

#include <Windows.h>

#include "imgui.h"

class SharedMemoryServer
{
public:
	struct Data
	{
		ImVec2 mouseCoordinates;
		bool sceneViewPortHasFocus;
	};

	static SharedMemoryServer& GetInstance();
	void Start();
	void Stop();
	Data* GetData();

private:
	SharedMemoryServer();
	SharedMemoryServer(const SharedMemoryServer& other) = delete;
	SharedMemoryServer& operator=(const SharedMemoryServer& other) = delete;

	HANDLE mapFile;
	Data* data;
};
