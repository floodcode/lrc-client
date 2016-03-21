#include "lrcdatahandler.hpp"
#include "websocket.hpp"
#include "settings.hpp"
#include "io.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <queue>
#include <random>
#include <map>

using namespace Services;
using namespace LRCData;

namespace
{
	using namespace std;

	atomic_bool isRunning = false;

	thread workerThread;

	mutex stateMutex;
	mutex dataQueueMutex;

	queue<ByteVector> dataQueue;

	uniform_int_distribution<uint32_t> tokenDistribution(1000, 9999);
	default_random_engine tokenGenerator;

	struct CacheBuffer
	{
		uint32_t token;
		time_t time;
		ByteVector data;
	};

	vector<CacheBuffer> cacheRead()
	{
		return vector<CacheBuffer>();
	}

	void cacheAdd(ByteVector data)
	{
		bool folderExist = io::directory::create(Settings::cacheDirectory);

		if (!folderExist)
		{
			return;
		}

		time_t currentTime = time(0);
		uint32_t token = tokenDistribution(tokenGenerator);

		stringstream cacheFilename;
		cacheFilename << Settings::cacheDirectory << "\\" << currentTime << "-" << token << ".bin";

		ofstream binaryOutput(cacheFilename.str(), ios::binary | ios::trunc);
		if (binaryOutput.is_open())
		{
			binaryOutput.write(reinterpret_cast<char *>(data.data()), data.size());
			binaryOutput.close();
		}
	}

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

				bool isDataSent = false;
				
				if (WebSocketSvc::IsConnected())
				{
					// Send data to server
					isDataSent = WebSocketSvc::Send(data);
				}

				if (!isDataSent)
				{
					// Cache data if wasn't sent
					cacheAdd(data);
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
	if (!isRunning.load())
	{
		return;
	}

	dataQueueMutex.lock();
	dataQueue.push(data);
	dataQueueMutex.unlock();
}