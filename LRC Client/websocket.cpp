#include "services.hpp"

#if SERVICE_WEBSOCKET_ENABLED

#include "winfx.hpp"
#include "wsclient.hpp"
#include <thread>
#include <mutex>
#include <atomic>

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
	void send(const std::string &message)
	{
		wsSendMutex.lock();
		if (ws->getReadyState() == WebSocketClient::OPEN)
		{
			ws->send(message);
		}
		wsSendMutex.unlock();
	}

	// Thread-safely send binary message to WebSocket server
	void sendBinary(std::vector<byte> message)
	{
		wsSendMutex.lock();
		if (ws->getReadyState() == WebSocketClient::OPEN)
		{
			ws->sendBinary(message);
		}
		wsSendMutex.unlock();
	}

	// Receiving incoming messages
	void handlemessage(const std::string &message)
	{
		// TODO: Pass message to command handler
		// send("Accepted - " + message);
	}

	// Try connect to WebSocket server
	bool tryconnect()
	{
		ws = WebSocketClient::from_url(WS_HOST);
		return ws != NULL;
	}

	// Automaticly connects to server and accepts incoming messages
	void worker()
	{
		while (isRunning)
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
				Sleep(1000 * WS_CONNECTION_DELAY_SEC);
			}
		}
	}
}

void WebSocket::Run()
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

void WebSocket::Stop()
{
	stateMutex.lock();

	if (!isRunning)
	{
		return;
	}

	wsWorkerThread.join();
	WSACleanup();
	isRunning = false;

	stateMutex.unlock();
}

bool WebSocket::Send(std::vector<byte> data)
{
	try
	{
		sendBinary(data);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool WebSocket::IsRunning()
{
	return isRunning;
}

#endif // SERVICE_WEBSOCKET_ENABLED