#include "services.hpp"

#if SERVICE_WEBSOCKET_ENABLED

#include "winfx.hpp"
#include "wsclient.hpp"
#include "settings.hpp"

#include <thread>
#include <mutex>
#include <atomic>

#include <iostream>

using namespace Services;
using wsclient::WebSocketClient;

namespace
{
	std::atomic_bool isRunning = false;
	std::atomic_bool isConnected = false;

	INT rc;
	WSADATA wsaData;
	WebSocketClient::pointer ws = NULL;

	std::thread wsWorkerThread;

	std::mutex stateMutex;
	std::mutex wsSendMutex;

	// Thread-safely send text messasge to WebSocket server
	bool send(const std::string &message)
	{
		bool isSent = false;
		wsSendMutex.lock();
		if (isConnected.load() && ws->getReadyState() == WebSocketClient::OPEN)
		{
			ws->send(message);
			isSent = true;
		}
		wsSendMutex.unlock();
		return isSent;
	}

	// Thread-safely send binary message to WebSocket server
	bool sendBinary(std::vector<byte> message)
	{
		bool isSent = false;
		wsSendMutex.lock();
		if (isConnected.load() && ws->getReadyState() == WebSocketClient::OPEN)
		{
			ws->sendBinary(message);
			isSent = true;
		}
		wsSendMutex.unlock();
		return isSent;
	}

	// Receiving incoming messages
	void handlemessage(const std::string &message)
	{
		std::cout << "[WerSocket][Server]: " << message << std::endl;

		// TODO: Pass message to command handler
		// send("Accepted - " + message);
	}

	// Try connect to WebSocket server
	bool tryconnect()
	{
		ws = WebSocketClient::from_url(Settings::WebSocketSvc::host);
		return ws != NULL;
	}

	// Automaticly connects to server and accepts incoming messages
	void worker()
	{
		while (isRunning.load())
		{
			isConnected = tryconnect();
			if (isConnected)
			{
				while (ws->getReadyState() != WebSocketClient::CLOSED)
				{
					ws->poll(100);
					ws->dispatch(handlemessage);
				}
				delete ws;
				isConnected = false;
			}
			else
			{
				Sleep(1000 * Settings::WebSocketSvc::connectionDelay);
			}
		}
	}
}

void WebSocketSvc::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	wsWorkerThread = std::thread(worker);

	stateMutex.unlock();
}

void WebSocketSvc::Stop()
{
	stateMutex.lock();

	if (!isRunning)
	{
		return;
	}

	wsWorkerThread.join();
	WSACleanup();
	isRunning.store(false);

	stateMutex.unlock();
}

bool WebSocketSvc::Send(std::vector<uint8_t> data)
{
	try
	{
		return sendBinary(data);
	}
	catch (...)
	{
		return false;
	}
}

bool WebSocketSvc::IsRunning()
{
	return isRunning.load();
}

#endif // SERVICE_WEBSOCKET_ENABLED