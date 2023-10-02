#pragma once

#include <thread>
#include <queue>
#include <functional>

#include "../Common.h"

#undef SendMessage

class PipeServer
{
public:
	using MessageCallback = std::function<void(const std::string& type, const std::string& content)>;
	using ConnectionCallback = std::function<void()>;

	~PipeServer();
	HitmanAbsolutionSDK_API static PipeServer& GetInstance();
	void Start();
	void Stop();
	void Update();
	HitmanAbsolutionSDK_API void SendMessage(const std::string& type, const std::string& content);
	void SetMessageCallback(MessageCallback messageCallback);
	void SetConnectionCallback(ConnectionCallback connectionCallback);
	HitmanAbsolutionSDK_API bool IsConnectedWithEditor() const;

private:
	PipeServer();
	PipeServer(const PipeServer& other) = delete;
	PipeServer& operator=(const PipeServer& other) = delete;

	HANDLE pipe;
	bool isConnectedWithEditor;
	std::thread thread;
	std::queue<std::string> queuedData;
	bool isRunning;
	unsigned int pendingMessageLength;
	MessageCallback messageCallback;
	ConnectionCallback connectionCallback;
	SRWLOCK srwLock;
};
