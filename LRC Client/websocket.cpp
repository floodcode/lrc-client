#include "services.hpp"

using wsclient::WebSocket;

void websocket::run()
{
	if (isRunning)
	{
		return;
	}

	isRunning = true;
	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	wsWorkerThread = std::thread(worker);
}

void websocket::stop()
{
	if (!isRunning)
	{
		return;
	}

	isRunning = false;
	wsWorkerThread.join();
	WSACleanup();
}

void websocket::worker()
{
	while (isRunning)
	{
		if (tryconnect())
		{
			int a = 2;
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll();
				ws->dispatch(handlemessage);
			}
			delete ws;
		}
		else
		{
			Sleep(1000 * 10);
		}
	}
}

void websocket::handlemessage(const std::string &message)
{
	int a = 2;
	ws->send("Accepted!");
}

bool websocket::tryconnect()
{
	ws = WebSocket::from_url(WEBSOCKET_HOST);
	return ws != NULL;
}