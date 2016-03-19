#include "binbuff.hpp"

namespace binbuff
{
	BinaryBuffer::BinaryBuffer()
	{
		_size = 0;
		_capacity = _defaultCapacity;
		_data = new uint8_t[_capacity];
	}

	void BinaryBuffer::Write(uint8_t byte)
	{
		if (_size == _capacity)
		{
			increaseCapacity();
		}

		_data[_size] = byte;
		_size++;
	}

	void BinaryBuffer::Write(uint8_t *data, size_t sz)
	{
		for (size_t i = 0; i < sz; ++i)
		{
			Write(data[i]);
		}
	}

	void BinaryBuffer::WriteLE(std::vector<uint8_t> data)
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			WriteLE(data[i]);
		}
	}

	void BinaryBuffer::WriteBE(std::vector<uint8_t> data)
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			WriteBE(data[i]);
		}
	}

	const uint8_t *BinaryBuffer::Data()
	{
		return _data;
	}

	std::vector<uint8_t> BinaryBuffer::GetBytes()
	{
		std::vector<uint8_t> result;
		result.reserve(_size);

		for (size_t i = 0; i < _size; ++i)
		{
			result.push_back(_data[i]);
		}

		return result;
	}

	size_t BinaryBuffer::Size()
	{
		return _size;
	}

	size_t BinaryBuffer::Capacity()
	{
		return _capacity;
	}

	std::ostream &operator<<(std::ostream &os, BinaryBuffer &data)
	{
		os << std::hex;
		for (size_t i = 0; i < data._size; ++i)
		{
			if (data._data[i] < 0x10)
			{
				os << '0';
			}

			os << static_cast<size_t>(data._data[i]) << " ";
		}
		os << std::dec;

		return os;
	}

	void BinaryBuffer::increaseCapacity()
	{
		uint8_t *oldData = _data;
		_data = new uint8_t[_capacity * 2];

		for (size_t i = 0; i < _capacity; i++)
		{
			_data[i] = oldData[i];
		}

		delete[] oldData;
		_capacity *= 2;
	}
}
