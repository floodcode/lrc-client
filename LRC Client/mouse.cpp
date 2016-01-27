#include "services.h"

void services::mouse::run()
{
	// Exit function if service is already running
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

void services::mouse::stop()
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

LRESULT CALLBACK services::mouse::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
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