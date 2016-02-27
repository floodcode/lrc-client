#include "services.hpp"
#include "winfx.hpp"
#include <cstring>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if !_DEBUG
	if (strcmp("run", lpCmdLine) != 0)
	{
		// Stop LRC if comand line arguments are not equals to "run"
		return 0;
	}
#endif // DEBUG == 0

	CreateEventW(NULL, FALSE, FALSE, L"12da89fa-068b-410d-bc86-9881394bc74f");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Prevent LRC from multiple execution
		return 0;
	}

	// Run all services
	Services::RunAll();

	// Windows message loop
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	// Stop all services
	Services::StopAll();

	return 0;
}