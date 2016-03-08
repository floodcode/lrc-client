#pragma once
#include "lrcdatatypes.hpp"
#include <vector>

namespace Services
{
	namespace WebSocketSvc
	{
		void Run();
		void Stop();
		bool IsRunning();

		bool Send(std::vector<uint8_t> data);
	}
}