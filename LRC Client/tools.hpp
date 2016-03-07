#pragma once
#include "winfx.hpp"
#include "lrcdatatypes.hpp"
#include <string>

namespace tools
{
	LRCData::WNDInfo GetWNDInfo(HWND hwnd);
	std::wstring GetWNDTitle(HWND hwnd);
	std::wstring GetWNDProcName(HWND hwnd);
}