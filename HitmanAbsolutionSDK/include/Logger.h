#pragma once

#include <Windows.h>
#include <iostream>
#include <format>

#include "Mutex.h"
#include "Connection/PipeServer.h"
#include "Common.h"

#undef GetCurrentTime

class Logger
{
public:
	enum class Level
	{
		Info,
		Warning,
		Error
	};

	struct Message
	{
		std::string ToString() const
		{
			std::string level = LevelToString(this->level);

			return std::format("ID: {}, Level: {}, Content: {}", id, level, content);
		}

		int id;
		Level level;
		std::string content;
	};

	~Logger();
	HitmanAbsolutionSDK_API static Logger& GetInstance();
	HitmanAbsolutionSDK_API static unsigned int LevelToNumber(const Level level);
	HitmanAbsolutionSDK_API static const char* LevelToString(const Level level);
	std::vector<Message>& GetMessages();
	void ClearMessage(const unsigned int index);
	void ClearAllMessages();
	HitmanAbsolutionSDK_API static std::string GetCurrentTime();
	static std::string GetLastError();

	template <typename... Args>
	void Log(const Level level, const std::string& format, const Args&... args)
	{
		ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);

		std::cout << LevelToString(level) << std::vformat(format, std::make_format_args(args...)) << std::endl;

		PipeServer& pipeServer = PipeServer::GetInstance();

		if (pipeServer.IsConnectedWithEditor())
		{
			std::string type = std::format("sdk_{}", LevelToNumber(level));
			std::string content = LevelToString(level) + std::vformat(format, std::make_format_args(args...));

			pipeServer.SendMessage(type, content);
		}

		Message message;

		message.id = static_cast<int>(messages.size());
		message.level = level;
		message.content = std::vformat(format, std::make_format_args(args...));

		messages.push_back(message);
	}

private:
	Logger();
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;

	FILE* file;
	SRWLOCK srwLock;
	std::vector<Message> messages;
};
