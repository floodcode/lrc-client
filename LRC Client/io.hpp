#pragma once
#include <Windows.h>
#include <fstream>
#include <string>

namespace io
{
	namespace directory
	{
		bool exist(std::string path);
		bool create(std::string path);
	}

	namespace file
	{
		bool write(std::string path, std::string text);
	}
}