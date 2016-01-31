#pragma once
#include <ctime>
#include <string>

namespace tools
{
	enum TimeFormat
	{
		file
	};
	std::string getTime(TimeFormat timeFormat);
}