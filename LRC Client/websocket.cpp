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
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll();
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

void websocket::handlemessage(const std::string &message)
{

}


void websocket::send(const std::string &message)
{
	wsSendMutex.lock();
	ws->send(message);
	wsSendMutex.unlock();
}

bool websocket::tryconnect()
{
	ws = WebSocket::from_url(WS_HOST);
	return ws != NULL;
}