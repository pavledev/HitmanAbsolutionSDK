#include <Windows.h>
#include <format>
#include <sstream>

#include "Connection/PipeServer.h"
#include "Global.h"
#include "Logger.h"
#include "Mutex.h"

PipeServer::PipeServer()
{
	pipe = nullptr;
	isConnectedWithEditor = false;
	thread = std::thread(&PipeServer::Update, this);
	isRunning = true;
	pendingMessageLength = 0;
	messageCallback = nullptr;

	InitializeSRWLock(&srwLock);
}

PipeServer::~PipeServer()
{
	if (isRunning)
	{
		Stop();
	}
}

PipeServer& PipeServer::GetInstance()
{
	static PipeServer instance;

	return instance;
}

void PipeServer::Start()
{
	pipe = CreateNamedPipeA("\\\\.\\pipe\\pipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, 0, nullptr);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create named pipe. Error: {}", Logger::GetLastError());
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Successfully created name pipe.");
	}
}

void PipeServer::Stop()
{
	isRunning = false;

	if (pipe)
	{
		CloseHandle(pipe);
	}
}

void PipeServer::Update()
{
	while (isRunning)
	{
		Sleep(5);

		if (pipe == nullptr)
		{
			continue;
		}

		if (!isConnectedWithEditor)
		{
			isConnectedWithEditor = ConnectNamedPipe(pipe, nullptr);

			if (isConnectedWithEditor)
			{
				Logger::GetInstance().Log(Logger::Level::Info, "Successfully connected editor with engine.");

				connectionCallback();
			}
		}

		AcquireSRWLockExclusive(&srwLock);

		if (!queuedData.empty())
		{
			std::string data;
			unsigned int dataLength = static_cast<unsigned int>(queuedData.front().size());

			data.append(reinterpret_cast<char*>(&dataLength), 4);
			data.append(queuedData.front().data(), dataLength);

			DWORD written;

			if (!WriteFile(pipe, data.data(), static_cast<DWORD>(data.size()), &written, nullptr))
			{
				Logger::GetInstance().Log(Logger::Level::Error, "Failed to send pipe data. Error: {}", Logger::GetLastError());

				//CloseHandle(pipe);
				//pipe = nullptr;
				isConnectedWithEditor = false;

				continue;
			}

			queuedData.pop();
		}

		ReleaseSRWLockExclusive(&srwLock);

		DWORD pendingBytes;

		if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &pendingBytes, nullptr))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to peek pipe. Error: {}", Logger::GetLastError());

			//CloseHandle(pipe);
			//pipe = nullptr;
			isConnectedWithEditor = false;

			continue;
		}

		if (pendingBytes <= 0)
		{
			continue;
		}

		if (pendingMessageLength == 0)
		{
			char messageLengthBuffer[4];
			DWORD messageLengthBufferLength;

			if (!ReadFile(pipe, messageLengthBuffer, 4, &messageLengthBufferLength, nullptr) || messageLengthBufferLength != 4)
			{
				DWORD lastError = GetLastError();

				if (lastError != ERROR_MORE_DATA)
				{
					Logger::GetInstance().Log(Logger::Level::Error, "Failed to read message length. Error: {}", Logger::GetLastError());

					//CloseHandle(pipe);
					//pipe = nullptr;
					isConnectedWithEditor = false;

					continue;
				}
			}

			pendingMessageLength = *(unsigned int*)&messageLengthBuffer[0];

			continue;
		}

		if (pendingMessageLength > 0 && pendingBytes < pendingMessageLength)
		{
			continue;
		}

		char* messageBuffer = new char[pendingMessageLength];
		DWORD messageBufferLength;

		// Read the handshake length.
		if (!ReadFile(pipe, messageBuffer, pendingMessageLength, &messageBufferLength, nullptr) || messageBufferLength != pendingMessageLength)
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to read message. Error: {}", Logger::GetLastError());

			delete[] messageBuffer;

			//CloseHandle(pipe);
			//pipe = nullptr;
			isConnectedWithEditor = false;

			continue;
		}

		std::string message = std::string(messageBuffer, pendingMessageLength);
		std::string type = message.substr(0, message.find("#"));
		std::string content = message.substr(message.find("#") + 1);

		pendingMessageLength = 0;
		delete[] messageBuffer;

		if (messageCallback)
		{
			messageCallback(type, content);
		}
	}
}

void PipeServer::SendMessage(const std::string& type, const std::string& content)
{
	ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);

	std::string data = std::format("{}#{}", type, content);

	queuedData.push(data);
}

void PipeServer::SetMessageCallback(MessageCallback messageCallback)
{
	this->messageCallback = messageCallback;
}

void PipeServer::SetConnectionCallback(ConnectionCallback connectionCallback)
{
	this->connectionCallback = connectionCallback;
}

bool PipeServer::IsConnectedWithEditor() const
{
	return isConnectedWithEditor;
}
