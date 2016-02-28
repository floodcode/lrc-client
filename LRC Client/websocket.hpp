#pragma once
#include "lrcdatatypes.hpp"
#include <vector>

namespace Services
{
	namespace WebSocket
	{
		void Run();
		void Stop();
		void Send(std::vector<byte> data);
		bool IsRunning();
	}
}