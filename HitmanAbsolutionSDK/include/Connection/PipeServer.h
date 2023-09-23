#pragma once

#include <thread>
#include <queue>
#include <functional>

#undef SendMessage

class PipeServer
{
public:
	using MessageCallback = std::function<void(const std::string& type, const std::string& content)>;
	using ConnectionCallback = std::function<void()>;

	~PipeServer();
	static PipeServer& GetInstance();
	void Start();
	void Stop();
	void Update();
	void SendMessage(const std::string& type, const std::string& content);
	void SetMessageCallback(MessageCallback messageCallback);
	void SetConnectionCallback(ConnectionCallback connectionCallback);
	bool IsConnectedWithEditor() const;

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
