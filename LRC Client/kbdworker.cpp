#include "kbdworker.h"
#include "winfx.hpp"
#include <chrono>

KeyboardWorker::KeyboardWorker()
{
	init();
	isRunning = true;
	queueWorkerThread = std::thread(&KeyboardWorker::queueWorker, this);
}

KeyboardWorker::~KeyboardWorker()
{
	isRunning = false;
	queueWorkerThread.join();
}

void KeyboardWorker::Add(lrcdata::VKInfo vkInfo)
{
	lrcdata::PartKeyboard pk;
	pk.subtype = LRCDATA_KEYBOARD_SUBTYPE_VKINFO;
	pk.vkInfo = vkInfo;

	pkQueueMutex.lock();
	pkQueue.push(pk);
	pkQueueMutex.unlock();
}


void KeyboardWorker::Add(lrcdata::WNDInfo wndInfo)
{
	lrcdata::PartKeyboard pk;
	pk.subtype = LRCDATA_KEYBOARD_SUBTYPE_WNDINFO;
	pk.wndInfo = wndInfo;

	pkQueueMutex.lock();
	pkQueue.push(pk);
	pkQueueMutex.unlock();
}

void KeyboardWorker::init()
{
	pkEvents = 0;
	pkRepeats = 0;
	pkListCursorBegin = 0;
	pkListCursor = 0;

	lastVKInfo.keyCode = 0;
	lastVKInfo.lang = 0;
	lastVKInfo.flags = 0;

	lastWNDInfo.title = L"";
	lastWNDInfo.processName = L"";

	pkList.clear();
}

void KeyboardWorker::queueWorker()
{
	while (isRunning)
	{
		pkQueueMutex.lock();
		bool pkQueueEmpty = pkQueue.empty();
		pkQueueMutex.unlock();
		if (!pkQueueEmpty)
		{
			lrcdata::PartKeyboard pk;

			pkQueueMutex.lock();
			pk = pkQueue.front();
			pkQueueMutex.unlock();

			processPartKeyboard(pk);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}

void KeyboardWorker::processPartKeyboard(lrcdata::PartKeyboard pk)
{
	if (pk.subtype == LRCDATA_KEYBOARD_SUBTYPE_VKINFO)
	{
		switch (pk.vkInfo.keyCode)
		{
		case VK_BACK:
			eraseLeft();
			break;
		case VK_DELETE:
			eraseRigth();
			break;
		case VK_UP:
			// Point cursor to the start
			pkListCursor = pkListCursorBegin;
			break;
		case VK_DOWN:
			// Point cursor to the end
			pkListCursor = pkList.size();
			break;
		case VK_LEFT:
			// Shift virtual-key cursor to left if cursor isn't at begining
			if (pkListCursor > pkListCursorBegin)
			{
				pkListCursor--;
			}
			break;
		case VK_RIGHT:
			// Shift virtual-key cursor to right if cursor isn't at end
			if (pkListCursor < pkList.size())
			{
				pkListCursor++;
			}
			break;
		}
	}
	else if (pk.subtype == LRCDATA_KEYBOARD_SUBTYPE_WNDINFO)
	{
		resetCursor();
	}
}

void KeyboardWorker::eraseLeft()
{
	if (pkListCursor > pkListCursorBegin)
	{
		std::list<lrcdata::PartKeyboard>::iterator it = pkList.begin();
		std::advance(it, pkListCursor - 1);
		pkList.erase(it);
		pkEvents--;
		pkListCursor--;
	}
}

void KeyboardWorker::eraseRigth()
{
	if (pkListCursor < pkList.size())
	{
		std::list<lrcdata::PartKeyboard>::iterator it = pkList.begin();
		std::advance(it, pkListCursor);
		pkList.erase(it);
		pkEvents--;
	}
}

void KeyboardWorker::resetCursor()
{
	pkListCursor = pkListCursorBegin = pkList.size();
}