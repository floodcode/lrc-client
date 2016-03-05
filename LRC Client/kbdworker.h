#pragma once
#include "lrcdatawriter.hpp"
#include <queue>
#include <list>
#include <mutex>

class KeyboardWorker
{
public:
	KeyboardWorker();
	~KeyboardWorker();
	void Add(lrcdata::VKInfo vkInfo);
	void Add(lrcdata::WNDInfo wndInfo);

private:
	bool isRunning;

	const size_t maxRepeats = 5;
	const size_t eventsToProcess = 100;

	std::queue<lrcdata::PartKeyboard> pkQueue;
	std::list<lrcdata::PartKeyboard> pkList;
	
	std::mutex pkQueueMutex;

	std::thread queueWorkerThread;

	size_t pkEvents;
	size_t pkRepeats;
	size_t pkListCursorBegin;
	size_t pkListCursor;

	lrcdata::VKInfo lastVKInfo;
	lrcdata::WNDInfo lastWNDInfo;

	void init();
	void queueWorker();
	void processPartKeyboard(lrcdata::PartKeyboard pk);
	void eraseLeft();
	void eraseRigth();
	void resetCursor();
};