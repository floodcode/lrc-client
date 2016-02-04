#pragma once
#include "winfx.hpp"
#include "wsclient.hpp"
#include <thread>

namespace websocket
{
	static INT rc;
	static WSADATA wsaData;
	static wsclient::WebSocket::pointer ws = NULL;

	static bool isRunning = false;

	static std::thread wsWorkerThread;

	void run();
	void stop();
	void worker();
	void handlemessage(const std::string &message);
	bool tryconnect();
}