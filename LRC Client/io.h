#pragma once
#include <Windows.h>
#include <string>

namespace io
{
	namespace directory
	{
		bool exist(std::string path);
		bool create(std::string path);
	}
}