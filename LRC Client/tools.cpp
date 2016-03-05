#include "tools.hpp"
#include <Tlhelp32.h>

const wchar_t wstrUnknown[8] = L"Unknown";

lrcdata::WNDInfo tools::GetWNDInfo(HWND hwnd)
{
	lrcdata::WNDInfo result;
	result.title.append(GetWNDTitle(hwnd));
	result.processName.append(GetWNDProcName(hwnd));
	return result;
}

std::wstring tools::GetWNDTitle(HWND hwnd)
{
	WCHAR title[256];
	int status = GetWindowTextW(hwnd, title, 256);
	return status != 0 ? title : wstrUnknown;
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
			WCHAR *procName = me.szExePath;

			int pos = -1;
			for (size_t i = 0; i < MAX_PATH; ++i)
			{
				if (procName[i] == '\\')
				{
					pos = i;
				}
			}
			return pos != -1 ? &procName[pos + 1] : wstrUnknown;
		}
		CloseHandle(hTH);
	}
	return wstrUnknown;
}