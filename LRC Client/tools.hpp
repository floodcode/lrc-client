#pragma once
#include "winfx.hpp"
#include "lrcdatatypes.hpp"
#include <string>

namespace tools
{
	lrcdata::WNDInfo GetWNDInfo(HWND hwnd);
	std::wstring GetWNDTitle(HWND hwnd);
	std::wstring GetWNDProcName(HWND hwnd);
}