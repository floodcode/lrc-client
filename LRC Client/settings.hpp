#pragma once
#include "services.hpp"
#include <string>
#include <mutex>

namespace Settings
{
	using std::mutex;
	using std::string;
	using std::wstring;

	static string UID = "";
	static wstring cacheDirectory = L"cache";
	static mutex mtx;
	
	namespace Log
	{
		static wstring directory = L"logs";
		static wstring filename = L"logs.txt";
	}

	namespace ServiceState
	{
		static const bool keyboard = SERVICE_KEYBOARD_ENABLED;
		static const bool mouse = SERVICE_MOUSE_ENABLED;
		static const bool clipboard = SERVICE_CLIPBOARD_ENABLED;
	}

	namespace WebSocketSvc
	{
		static string host = "ws://localhost:8080/";
		static size_t connectionDelay = 10;
	}

	namespace KeyboardSvc
	{
		static size_t eventsToProcess = 5;
		static size_t maxRepeats = 5;
	}

	namespace ClipboardSvc
	{
		static size_t eventsToProcess = 5;
	}
}