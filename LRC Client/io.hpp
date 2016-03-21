#pragma once
#include "winfx.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace io
{
	namespace directory
	{
		bool exist(std::string path);
		bool create(std::string path);
	}

	namespace file
	{
		std::vector<std::wstring> list(std::wstring directory);
	}
}