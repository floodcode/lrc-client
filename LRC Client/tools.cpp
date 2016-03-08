#include "tools.hpp"
#include <Tlhelp32.h>
#include <ctime>

const wchar_t wstrUnknown[8] = L"Unknown";

LRCData::WNDInfo tools::GetWNDInfo(HWND hwnd)
{
	LRCData::WNDInfo result;
	result.time = static_cast<uint32_t>(time(0));
	result.title.append(GetWNDTitle(hwnd));
	result.process.append(GetWNDProcName(hwnd));
	return result;
}

std::wstring tools::GetWNDTitle(HWND hwnd)
{
	WCHAR title[256];
	int status = GetWindowTextW(hwnd, title, 256);

	if (status == 0 || title[0] == 0)
	{
		return wstrUnknown;
	}
	else
	{
		return title;
	}
}

std::wstring tools::GetWNDProcName(HWND hwnd)
{
	DWORD dwPID;
	GetWindowThreadProcessId(hwnd, &dwPID);

	HANDLE hTH = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	if (hTH != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W me = { sizeof(MODULEENTRY32W) };
		if (Module32FirstW(hTH, &me))
		{
			WCHAR *procPath = me.szExePath;

			int pos = -1;
			for (size_t i = 0; i < MAX_PATH; ++i)
			{
				if (procPath[i] == '\\')
				{
					pos = i;
				}
			}
			
			if (pos == -1)
			{
				return wstrUnknown;
			}
			else
			{
				WCHAR *procName = &procPath[pos + 1];

				if (procName[0] == 0)
				{
					return wstrUnknown;
				}
				else
				{
					return procName;
				}
			}
		}
		CloseHandle(hTH);
	}
	return wstrUnknown;
}