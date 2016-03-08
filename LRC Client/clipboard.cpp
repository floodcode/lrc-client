#include "services.hpp"

#if SERVICE_CLIPBOARD_ENABLED

#include <atomic>
#include <mutex>

using namespace Services;

namespace
{
	std::atomic_bool isRunning = false;

	std::mutex stateMutex;
}

void ClipboardSvc::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	stateMutex.unlock();
}

void ClipboardSvc::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	stateMutex.unlock();
}

bool ClipboardSvc::IsRunning()
{
	return isRunning.load();
}

#endif