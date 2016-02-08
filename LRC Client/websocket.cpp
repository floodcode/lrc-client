#include "services.hpp"

using wsclient::WebSocket;

void websocket::run()
{
	if (isRunning)
	{
		return;
	}

	isRunning = true;
	jsonHelper.setId("fe6340be87fd5e43b7f0cac5741e76205dd69a68b2024fda16c696848a720f7a");
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
			send(jsonHelper.);
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
	send(jsonHelper.getInfo("Data was accepted"));
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