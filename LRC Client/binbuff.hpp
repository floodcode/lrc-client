#ifndef BINARY_BUFFER_HPP
#define BINARY_BUFFER_HPP
#include <cstdint>
#include <vector>
#include <ostream>

namespace binbuff
{
	class BinaryBuffer
	{
	public:
		BinaryBuffer();
		void Write(uint8_t byte);
		void Write(uint8_t *data, size_t sz);
		void WriteLE(std::vector<uint8_t> data);
		void WriteBE(std::vector<uint8_t> data);

		template<typename T>
		void WriteLE(T data)
		{
			if (_size + sizeof(data) >= _capacity)
			{
				increaseCapacity();
			}

			T *dataPtr = reinterpret_cast<T *>(&_data[_size]);
			*dataPtr = data;
			_size += sizeof(T);
		}

		template<typename T>
		void WriteBE(T data)
		{
			uint8_t *byteDataPtr = reinterpret_cast<uint8_t *>(&data);
			for (size_t i = sizeof(T); i > 0; --i)
			{
				Write(byteDataPtr[i - 1]);
			}
		}

		template<typename T>
		void WriteLE(T *data, size_t sz)
		{
			for (size_t i = 0; i < sz; ++i)
			{
				WriteLE<T>(data[i]);
			}
		}

		template<typename T>
		void WriteBE(T *data, size_t sz)
		{
			for (size_t i = 0; i < sz; ++i)
			{
				WriteBE<T>(data[i]);
			}
		}

		template<typename T>
		T ReadLE(size_t offset)
		{
			if (offset + sizeof(T) > _size)
			{
				throw std::out_of_range("offset is out of range");
			}

			T *resPtr = reinterpret_cast<T *>(&_data[offset]);
			return static_cast<T>(*resPtr);
		}

		template<typename T>
		T ReadBE(size_t offset)
		{
			if (offset + sizeof(T) > _size)
			{
				throw std::out_of_range("offset is out of range");
			}

			T result = 0;
			result |= _data[offset];
			for (size_t i = offset + 1; i < offset + sizeof(T); ++i)
			{
				result <<= 8;
				result |= _data[i];
			}
			return result;
		}

		const uint8_t *Data();
		std::vector<uint8_t> GetBytes();
		size_t Size();
		size_t Capacity();
		friend std::ostream &operator<<(std::ostream &stream, BinaryBuffer &data);

	private:
		uint8_t *_data;
		size_t _capacity;
		size_t _size;

		static const size_t _defaultCapacity = 64U;

		void increaseCapacity();

	};
}

#endif // BINARY_BUFFER_HPP
