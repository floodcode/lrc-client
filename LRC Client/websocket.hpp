#pragma once
#include "winfx.hpp"
#include "wsclient.hpp"
#include <thread>
#include <mutex>

namespace websocket
{
	static INT rc;
	static WSADATA wsaData;
	static wsclient::WebSocket::pointer ws = NULL;

	static bool isRunning = false;

	static std::thread wsWorkerThread;
	static std::mutex wsSendMutex;

	void run();
	void stop();
	void worker();
	void send(const std::string &message);
	void handlemessage(const std::string &message);
	bool tryconnect();
}