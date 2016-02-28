#include "services.hpp"

#if SERVICE_CLIPBOARD_ENABLED

using namespace Services;

namespace
{
	bool isRunning = false;
}

void Clipboard::Run()
{
	if (isRunning)
	{
		return;
	}

	isRunning = true;
}

void Clipboard::Stop()
{
	if (!isRunning)
	{
		return;
	}

	isRunning = false;
}

bool Clipboard::IsRunning()
{
	return isRunning;
}

#endif