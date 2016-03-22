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
	queue<wstring> cacheQueue;

	uniform_int_distribution<uint32_t> tokenDistribution(1000, 9999);
	default_random_engine tokenGenerator;

	void loadCache()
	{
		vector<wstring> files = io::file::list(Settings::cacheDirectory);

		if (files.size() == 0)
		{
			return;
		}

		for (wstring filename : files)
		{
			wstringstream relativeFilename;
			relativeFilename << Settings::cacheDirectory << L"\\" << filename;

			cacheQueue.push(relativeFilename.str());
		}
	}

	void cacheAdd(ByteVector data)
	{
		bool folderExist = io::directory::create(Settings::cacheDirectory);

		if (!folderExist)
		{
			return;
		}

		time_t cacheTime = time(0);
		uint32_t token = tokenDistribution(tokenGenerator);

		wstringstream cacheFilename;
		cacheFilename << Settings::cacheDirectory << "\\" << cacheTime << "-" << token << ".bin";

		ofstream binaryOutput(cacheFilename.str(), ios::binary | ios::trunc);
		if (binaryOutput.is_open())
		{
			binaryOutput.write(reinterpret_cast<char *>(data.data()), data.size());
			binaryOutput.close();
			cacheQueue.push(cacheFilename.str());
		}

		wcout << L"[Cache] Data was cached to '" << cacheFilename.str() << L"' (" << data.size() << L" bytes)" << endl;
	}

	void worker()
	{
		while (isRunning.load())
		{
			// Websocket connection is estabilished
			if (WebSocketSvc::IsConnected())
			{
				// Cache isn't empty
				if (!cacheQueue.empty())
				{
					// Try to send cached data
					wstring cacheFilename = cacheQueue.front();

					// Read cache data from file and try to send it
					ifstream cacheInput(cacheFilename, ios::binary | ios::ate);
					if (cacheInput.is_open())
					{
						ifstream::pos_type pos = cacheInput.tellg();

						ByteVector data(pos);

						cacheInput.seekg(0, ios::beg);
						cacheInput.read(reinterpret_cast<char *>(&data[0]), pos);
						cacheInput.close();

						if (WebSocketSvc::Send(data))
						{
							// Delete cached file if data sent
							io::file::remove(cacheFilename);
							cacheQueue.pop();
						}
					}
				}
				else
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

						// Cache data if wasn't sent
						if (!WebSocketSvc::Send(data))
						{
							cacheAdd(data);
						}
					}
				}
			}
			else
			{
				// Cache data from queue
				dataQueueMutex.lock();
				bool dataQueueEmpty = dataQueue.empty();
				dataQueueMutex.unlock();

				if (!dataQueueEmpty)
				{
					dataQueueMutex.lock();
					ByteVector data = dataQueue.front();
					dataQueue.pop();
					dataQueueMutex.unlock();

					cacheAdd(data);
				}
			}
			this_thread::sleep_for(chrono::milliseconds(200));
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

	loadCache();

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