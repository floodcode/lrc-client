#include "tools.hpp"

std::string tools::getTime(tools::TimeFormat timeFormat)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[32];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	switch (timeFormat)
	{
	case TimeFormat::file:
		strftime(buffer, 32, "%y.%m.%d %H-%M-%S", timeinfo);
		break;
	}

	std::string result(buffer);

	return result;
}