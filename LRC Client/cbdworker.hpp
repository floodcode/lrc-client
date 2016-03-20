#pragma once

#include "lrcdatawriter.hpp"
#include "websocket.hpp"

namespace Services
{
	namespace ClipboardWorker
	{
		void Run();
		void Stop();
		bool IsRunning();

		void Add(LRCData::Clipboard cbd);
	}
}