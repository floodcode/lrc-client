#pragma once

#include "lrcdatawriter.hpp"
#include "websocket.hpp"

namespace Services
{
	namespace KeyboardWorker
	{
		void Run();
		void Stop();
		bool IsRunning();

		void Add(LRCData::VKInfo vkInfo);
		void Add(LRCData::WNDInfo wndInfo);
	}
}