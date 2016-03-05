#pragma once
#include "winfx.hpp"
#include <fstream>
#include <string>

namespace io
{
	namespace directory
	{
		bool exist(std::string path);
		bool create(std::string path);
	}
}