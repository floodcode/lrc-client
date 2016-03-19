#include "services.hpp"

#if SERVICE_CLIPBOARD_ENABLED

#include "winfx.hpp"

#include <iostream>
#include <string>

#include <atomic>
#include <mutex>

using namespace Services;

namespace
{
	std::atomic_bool isRunning = false;

	std::mutex stateMutex;

	std::wstring cbdClassName;
	HWND hwnd;

	LRESULT CALLBACK CbdWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLIPBOARDUPDATE:
			std::cout << "Clipboard text changed." << std::endl;
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
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
		hwnd = CreateWindowExW(
			NULL,
			cbdClassName.c_str(),
			cbdClassName.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
			HWND_MESSAGE, NULL, NULL, NULL);

		if (hwnd == NULL)
		{
			UnregisterClassW(cbdClassName.c_str(), NULL);
			return false;
		}

		ShowWindow(hwnd, SW_HIDE);
		UpdateWindow(hwnd);

		// Adding clipboard listener
		BOOL res = AddClipboardFormatListener(hwnd);
		if (res == FALSE)
		{
			DestroyWindow(hwnd);
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

		DestroyWindow(hwnd);
		UnregisterClassW(cbdClassName.c_str(), NULL);
		RemoveClipboardFormatListener(hwnd);
		cbdClassName.clear();
	}
}

void ClipboardSvc::Run()
{
	stateMutex.lock();
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
	isRunning.store(false);

	DestroyClipboardListener();

	stateMutex.unlock();
}

bool ClipboardSvc::IsRunning()
{
	return isRunning.load();
}

#endif