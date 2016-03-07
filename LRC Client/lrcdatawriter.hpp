#ifndef LRCDATA_LRCDATAWRITER
#define LRCDATA_LRCDATAWRITER
#include "binarywriter.hpp"
#include "lrcdatatypes.hpp"
#include <string>
#include <list>

namespace LRCData
{
	class LRCDataWriter
	{
	public:
		LRCDataWriter(std::string id);
		bool WriteData(std::string filename, std::list<PartKeyboard> data);
		bool WriteData(std::string filename, std::list<PartClipboard> data);
		ByteVector GetBytes(std::list<PartKeyboard> data);
		ByteVector GetBytes(std::list<PartClipboard> data);
	private:
		Header header;
		ByteVector getHeaderBytes();
		void initHeader();
	};
}

#endif // !LRCDATA_LRCDATAWRITER
