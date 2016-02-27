#pragma once

namespace Services
{
	namespace WebSocket
	{
		void Run();
		void Stop();
		bool IsRunning();
	}
}