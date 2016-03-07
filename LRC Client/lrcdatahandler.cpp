#include "lrcdatahandler.h"
#include "websocket.hpp"

#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>

#include <queue>
#include <fstream>
#include <iostream>

using namespace Services;
using namespace LRCData;

namespace
{
	std::atomic_bool isRunning = false;

	std::thread workerThread;

	std::mutex stateMutex;
	std::mutex dataQueueMutex;

	std::queue<ByteVector> dataQueue;

	void worker()
	{
		while (isRunning.load())
		{
			dataQueueMutex.lock();
			bool dataQueueEmpty = dataQueue.empty();
			dataQueueMutex.unlock();

			if (!dataQueueEmpty)
			{
				dataQueueMutex.lock();
				ByteVector data = dataQueue.front();
				dataQueue.pop();
				dataQueueMutex.unlock();

				// Dump data
				std::ofstream output("dump.bin", std::ios::trunc);

				if (output.is_open())
				{
					output.write((const char *)data.data(), data.size());
					output.close();
				}
				
				// Send data
				std::cout << "[WebSocket] Sending data (" << data.size() << " bytes)" << std::endl;
				bool isDataSent = WebSocket::Send(data);
				std::cout << "[WebSocket] " << (isDataSent ? "Data was successfully sent" : "Data wasn't sent") << std::endl;

				if (!isDataSent)
				{
					// Cache file
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			else
			{
				// TODO: Check data cache
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}
}

void LRCDataHandler::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	workerThread = std::thread(worker);

	stateMutex.unlock();
}

void LRCDataHandler::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	workerThread.join();

	stateMutex.unlock();
}

bool LRCDataHandler::IsRunning()
{
	return isRunning.load();
}

void LRCDataHandler::Process(ByteVector data)
{
	dataQueueMutex.lock();
	dataQueue.push(data);
	dataQueueMutex.unlock();
}