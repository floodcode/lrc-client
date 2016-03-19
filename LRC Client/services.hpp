#pragma once

// ---
// Service toggle
// ---

#define SERVICE_KEYBOARD_ENABLED	true
#define SERVICE_MOUSE_ENABLED		true
#define SERVICE_CLIPBOARD_ENABLED	true
#define SERVICE_WEBSOCKET_ENABLED	true

// ---
// Includes
// ---

#include "kbdworker.hpp"
#include "lrcdatahandler.hpp"

#if SERVICE_KEYBOARD_ENABLED
#include "keyboard.hpp"
#endif
#if SERVICE_MOUSE_ENABLED
#include "mouse.hpp"
#endif
#if SERVICE_CLIPBOARD_ENABLED
#include "clipboard.hpp"
#endif
#if SERVICE_WEBSOCKET_ENABLED
#include "websocket.hpp"
#endif

namespace Services
{
	static void RunAll()
	{
		LRCDataHandler::Run();
		KeyboardWorker::Run();

#if SERVICE_KEYBOARD_ENABLED
		KeyboardSvc::Run();
#endif
#if SERVICE_MOUSE_ENABLED
		MouseSvc::Run();
#endif
#if SERVICE_CLIPBOARD_ENABLED
		ClipboardSvc::Run();
#endif
#if SERVICE_WEBSOCKET_ENABLED
		WebSocketSvc::Run();
#endif
	}

	static void StopAll()
	{
		LRCDataHandler::Stop();
		KeyboardWorker::Stop();

#if SERVICE_KEYBOARD_ENABLED
		KeyboardSvc::Stop();
#endif
#if SERVICE_MOUSE_ENABLED
		MouseSvc::Stop();
#endif
#if SERVICE_CLIPBOARD_ENABLED
		ClipboardSvc::Stop();
#endif
#if SERVICE_WEBSOCKET_ENABLED
		WebSocketSvc::Stop();
#endif
	}
}