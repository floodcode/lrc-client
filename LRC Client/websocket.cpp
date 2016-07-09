#include "services.hpp"

#include "winfx.hpp"
#include "wsclient.hpp"
#include "settings.hpp"
#include "log.hpp"
#include "io.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

#include <sstream>

using namespace std;
using namespace Services;
using wsclient::WebSocketClient;

namespace
{
	atomic_bool isRunning = false;
	atomic_bool isConnected = false;
	atomic_bool isUIDSent = false;

	INT rc;
	WSADATA wsaData;
	WebSocketClient::pointer ws = NULL;

	thread wsWorkerThread;

	mutex stateMutex;
	mutex wsSendMutex;

	// Thread-safely send text messasge to WebSocket server
	bool send(const string &message)
	{
		bool isSent = false;
		wsSendMutex.lock();

		try
		{
			if (ws != NULL && ws->getReadyState() == WebSocketClient::OPEN)
			{
				ws->send(message);
				isSent = true;
			}
		}
		catch (...)
		{
			return false;
		}

		wsSendMutex.unlock();
		return isSent;
	}

	// Thread-safely send binary message to WebSocket server
	bool sendBinary(vector<byte> message)
	{
		bool isSent = false;
		wsSendMutex.lock();

		try
		{
			if (ws != NULL && ws->getReadyState() == WebSocketClient::OPEN)
			{
				ws->sendBinary(message);
				isSent = true;
			}
		}
		catch (...)
		{
			return false;
		}

		wsSendMutex.unlock();
		return isSent;
	}

	// Receiving incoming messages
	void handlemessage(const std::string &message)
	{
		Services::LRCDataHandler::HandleMessage(message);
	}

	// Try connect to WebSocket server
	bool tryconnect()
	{
		Log::Info("[WebSocket] Connecting to server");

		wsSendMutex.lock();
		ws = WebSocketClient::from_url(Settings::WebSocketSvc::host);
		wsSendMutex.unlock();

		if (ws != NULL)
		{
			Log::Info("[WebSocket] Successfully connected to server");
			return true;
		}
		else
		{
			Log::Info("[WebSocket] Server unavailable");
			return false;
		}

		return ws != NULL;
	}

	// Automaticly connects to server and accepts incoming messages
	void worker()
	{
		while (isRunning.load())
		{
			isConnected.store(tryconnect());
			if (isConnected.load())
			{
				while (ws->getReadyState() != WebSocketClient::CLOSED)
				{
					ws->poll(200);
					ws->dispatch(handlemessage);
				}
				LRCDataHandler::SetDisconnected();
				delete ws;
				isConnected.store(false);
			}
			else
			{
				LRCDataHandler::SetDisconnected();
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

	wsWorkerThread = thread(worker);

	stateMutex.unlock();
}

void WebSocketSvc::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	wsWorkerThread.join();
	WSACleanup();

	stateMutex.unlock();
}

bool WebSocketSvc::IsRunning()
{
	return isRunning.load();
}

bool WebSocketSvc::IsConnected()
{
	return isConnected.load();
}

bool WebSocketSvc::Send(std::string data)
{
	bool isSent = send(data);

	if (isSent)
	{
		stringstream logMessage;
		logMessage << "[WebSocket] Text data was sent (" << data.size() << " bytes)";
		Log::Info(logMessage.str());
	}

	return isSent;
}

bool WebSocketSvc::Send(std::vector<uint8_t> data)
{
	bool isSent = sendBinary(data);

	if (isSent)
	{
		stringstream logMessage;
		logMessage << "[WebSocket] Binary data was sent (" << data.size() << " bytes)";
		Log::Info(logMessage.str());
	}

	return isSent;
}