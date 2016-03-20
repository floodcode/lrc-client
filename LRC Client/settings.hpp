#pragma once
#include <string>


namespace Settings
{
	using std::string;

	static string sha256ID = "fe6340be87fd5e43b7f0cac5741e76205dd69a68b2024fda16c696848a720f7a";

	namespace WebSocketSvc
	{
		static string host = "ws://localhost:8080/";
		static size_t connectionDelay = 60;
	}

	namespace KeyboardSvc
	{
		static size_t eventsToProcess = 10;
		static size_t maxRepeats = 5;
	}

	namespace ClipboardSvc
	{
		static size_t eventsToProcess = 10;
	}
}