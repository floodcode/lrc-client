#pragma once
#include <Windows.h>

namespace services
{
	namespace mouse
	{
		static bool isRunning = false;

		static HHOOK hhkLowLevelMouse = NULL;

		LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

		void run();
		void stop();
	}
}