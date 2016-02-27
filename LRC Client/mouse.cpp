#include "services.hpp"

#if MOUSE_SERVICE

#include "winfx.hpp"

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

#endif // MOUSE_SERVICE