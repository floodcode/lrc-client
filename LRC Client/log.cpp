#include "log.hpp"
#include "settings.hpp"
#include "io.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#include <mutex>

using namespace std;

namespace
{
	mutex logMutex;

	void logText(const wstring &logType, const wstring &message)
	{
		logMutex.lock();

		if (!io::directory::exist(Settings::Log::directory))
		{
			bool created = io::directory::create(Settings::Log::directory);
			if (!created)
			{
				return;
			}
		}

		tm rawtm;
		time_t rawtime = time(0);
		gmtime_s(&rawtm, &rawtime);
		char timeBuffer[32];
		strftime(timeBuffer, 32, "%y/%m/%d %H:%M:%S", &rawtm);

		wstringstream logText;
		logText << L"[" << timeBuffer << L"][" << logType << L"]" << message << endl;

#if _DEBUG
		wcout << logText.str();
#endif

		wstringstream filePath;
		filePath << Settings::Log::directory << L"\\" << Settings::Log::filename;

		wofstream output(filePath.str(), ios::app);
		if (output.is_open())
		{
			output << logText.str();
			output.close();
		}
		else
		{
			return;
		}

		logMutex.unlock();
	}

	void logText(const wstring &logType, const string &message)
	{
		logText(logType, wstring(message.begin(), message.end()));
	}

}

void Log::Info(const string &message)
{
	logText(L"Info", message);
}

void Log::Info(const wstring &message)
{
	logText(L"Info", message);
}

void Log::Error(const string &message)
{
	logText(L"Error", message);
}

void Log::Error(const wstring &message)
{
	logText(L"Error", message);
}