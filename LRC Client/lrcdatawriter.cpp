#include "lrcdatawriter.hpp"
#include <iterator>
#include <locale>
#include <codecvt>

namespace LRCData
{
	using namespace binbuff;

	template<typename T>
	ByteVector getBytes(T data)
	{
		ByteVector bvResult;

		for (size_t i = sizeof(T); i > 0; --i)
		{
			bvResult.push_back(static_cast<uint8_t>(data >> ((i - 1) * 8)));
		}

		return bvResult;
	}

	ByteVector getStrBytes(std::wstring text)
	{
		ByteVector bvResult;

		std::wstring_convert<std::codecvt_utf8<wchar_t>> textConvert;
		std::string strText = textConvert.to_bytes(text);

		ByteVector bvSize = getBytes(static_cast<uint32_t>(strText.size()));

		std::copy(bvSize.begin(), bvSize.end(), std::back_inserter(bvResult));
		std::copy(strText.begin(), strText.end(), std::back_inserter(bvResult));

		return bvResult;
	}

	// LRCDataWriter

	LRCDataWriter::LRCDataWriter(std::string id)
	{
		initHeader();
		if (id.length() != 64)
		{
			throw "ID length should be 64!";
		}

		for (size_t i = 0; i < 64; ++i)
		{
			header.id[i] = static_cast<uint8_t>(id[i]);
		}

		header.type = 0x0;
	}


	ByteVector LRCDataWriter::GetBytes(std::vector<Keyboard> data)
	{
		BinaryBuffer buffer;

		// data.count
		buffer.WriteBE<uint32_t>(data.size());

		// data.items[n]
		for each (Keyboard kbd in data)
		{
			// data.items[n].wndInfo
			buffer.WriteBE<uint32_t>(kbd.wndInfo.time);
			buffer.WriteBE(getStrBytes(kbd.wndInfo.process));
			buffer.WriteBE(getStrBytes(kbd.wndInfo.title));

			// data.items[n].count
			buffer.WriteBE<uint32_t>(kbd.keys.size());

			// data.items[n].keys[n]
			for each (VKInfo vkInfo in kbd.keys)
			{
				// data.items[n].keys[n]
				buffer.WriteBE<uint32_t>(vkInfo.keyCode);
				buffer.WriteBE<uint16_t>(vkInfo.lang);
				buffer.WriteBE<uint8_t>(vkInfo.flags);
			}
		}

		ByteVector bvData = buffer.GetBytes();

		header.type = Type::keyboard;
		header.length = bvData.size();

		ByteVector bvResult = getHeaderBytes();

		std::copy(bvData.begin(), bvData.end(), std::back_inserter(bvResult));

		return bvResult;
	}

	ByteVector LRCDataWriter::GetBytes(std::vector<Clipboard> data)
	{
		BinaryBuffer buffer;

		// data.count
		buffer.WriteBE<uint32_t>(data.size());

		// data.items[n]
		for each (Clipboard cbd in data)
		{
			// data.items[n].wndInfo
			buffer.WriteBE<uint32_t>(cbd.wndInfo.time);
			buffer.WriteBE(getStrBytes(cbd.wndInfo.process));
			buffer.WriteBE(getStrBytes(cbd.wndInfo.title));

			// data.items[n].data
			buffer.WriteBE(getStrBytes(cbd.data));
		}

		ByteVector bvData = buffer.GetBytes();

		header.type = Type::clipboard;
		header.length = bvData.size();

		ByteVector bvResult = getHeaderBytes();

		std::copy(bvData.begin(), bvData.end(), std::back_inserter(bvResult));

		return bvResult;
	}

	ByteVector LRCDataWriter::getHeaderBytes()
	{
		BinaryBuffer buffer;
		buffer.WriteBE(header.signature);
		buffer.WriteBE(header.version);
		buffer.WriteBE(header.id, 64);
		buffer.WriteBE(header.type);
		buffer.WriteBE(header.length);
		return buffer.GetBytes();
	}

	void LRCDataWriter::initHeader()
	{
		this->header.signature = 0x539;
		this->header.version = 0x1;
	}

	// !LRCDataWriter
}