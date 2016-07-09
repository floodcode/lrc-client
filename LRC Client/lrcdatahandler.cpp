#include "lrcdatahandler.hpp"
#include "websocket.hpp"
#include "settings.hpp"
#include "io.hpp"
#include "log.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

#include <fstream>
#include <iostream>
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
	atomic_bool isUIDSent = false;

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

		wstringstream logMessage;
		logMessage << L"[Cache] Data was cached to '" << cacheFilename.str() << L"' (" << data.size() << L" bytes)";
		Log::Info(logMessage.str());

	}

	bool getUID()
	{
		return WebSocketSvc::Send("{\"name\":\"get-uid\", \"data\":\"\"}");
	}

	bool sendUID()
	{
		stringstream json;
		Settings::mtx.lock();
		json << "{\"name\":\"set-uid\", \"data\":\"" << Settings::UID << "\"}";
		Settings::mtx.unlock();
		bool isSent = WebSocketSvc::Send(json.str());
		isUIDSent.store(isSent);
		return isSent;
	}

	void worker()
	{
		while (isRunning.load())
		{
			// Websocket connection is estabilished
			if (WebSocketSvc::IsConnected())
			{
				// Send UID if it isn't sent
				if (!isUIDSent.load())
				{
					Settings::mtx.lock();
					bool uidSet = (Settings::UID.length() == 64);
					Settings::mtx.unlock();


					if (uidSet)
					{
						sendUID();
					}
					else
					{
						getUID();
					}

					this_thread::sleep_for(chrono::milliseconds(200));
					continue;
				}

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

void bootstrap()
{
	if (!io::directory::exist("cfg"))
	{
		io::directory::create("cfg");
	}
	else
	{
		if (io::file::exist("cfg\\uid"))
		{
			std::ifstream input("cfg\\uid");
			if (input.is_open())
			{
				std::stringstream buffer;
				buffer << input.rdbuf();
				input.close();

				if (buffer.str().length() == 64)
				{
					Settings::mtx.lock();
					Settings::UID = buffer.str();
					Settings::mtx.unlock();
				}
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

	bootstrap();
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


void LRCDataHandler::HandleMessage(const string &message)
{
	stringstream logMessage;
	logMessage << "[WebSocket] Server: " << message;
	Log::Info(logMessage.str());

	string::size_type loc = message.find(":", 0);
	if (loc == string::npos)
	{
		return;
	}

	string cmd = message.substr(0, loc);
	string req = message.substr(loc + 1, message.length() - loc);

	if (cmd == "uid")
	{
		Settings::mtx.lock();
		Settings::UID = req;
		Settings::mtx.unlock();

		if (!io::directory::exist("cfg"))
		{
			io::directory::create("cfg");
		}
		else
		{
			std::ofstream output("cfg\\uid", ios::trunc);
			if (output.is_open())
			{
				output << req;
				output.close();
			}
		}
	}
}


void LRCDataHandler::SetDisconnected()
{
	isUIDSent.store(false);
}