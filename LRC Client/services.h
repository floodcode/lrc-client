#pragma once
// Main settings
#define DEBUG true

// Keylogger settings
#define KEYLOGGER_ENABLED true
#define KEYLOGGER_MAX_REPEATS 10

// Includes
#if KEYLOGGER_ENABLED
#include "keylogger.h"
#endif // KEYLOGGER_ENABLED