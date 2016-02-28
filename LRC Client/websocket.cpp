#include "services.hpp"

#if SERVICE_WEBSOCKET_ENABLED

#include "winfx.hpp"
#include "wsclient.hpp"
#include <thread>
#include <mutex>

using namespace Services;
using wsclient::WebSocketClient;

namespace
{
	bool isRunning = false;

	INT rc;
	WSADATA wsaData;
	WebSocketClient::pointer ws = NULL;

	std::thread wsWorkerThread;
	std::mutex wsSendMutex;

	// Thread-safely send text messasge to WebSocket server
	void send(const std::string &message)
	{
		wsSendMutex.lock();
		ws->send(message);
		wsSendMutex.unlock();
	}

	// Thread-safely send binary message to WebSocket server
	void sendBinary(std::vector<byte> message)
	{
		wsSendMutex.lock();
		ws->sendBinary(message);
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
			if (tryconnect())
			{
				while (ws->getReadyState() != WebSocketClient::CLOSED)
				{
					ws->poll(100);
					ws->dispatch(handlemessage);
				}
				delete ws;
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
	if (isRunning)
	{
		return;
	}

	isRunning = true;
	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	wsWorkerThread = std::thread(worker);
}

void WebSocket::Stop()
{
	if (!isRunning)
	{
		return;
	}

	isRunning = false;
	wsWorkerThread.join();
	WSACleanup();
}

void WebSocket::Send(std::vector<byte> data)
{
	sendBinary(data);
}

bool WebSocket::IsRunning()
{
	return isRunning;
}

#endif // SERVICE_WEBSOCKET_ENABLED