#ifndef BINPP_BINARYWRITER
#define BINPP_BINARYWRITER
#include <string>
#include <vector>
#include <fstream>

namespace binpp
{
	class BinaryWriter
	{
	public:
		BinaryWriter();
		BinaryWriter(size_t cp);
		~BinaryWriter();
		void Append(uint8_t data);
		void GetBytes(uint8_t *out, size_t count);
		void Clear();
		void Clear(size_t capacity);
		bool Save(std::string filename);
		size_t Size();
		size_t Capacity();
		uint8_t *Data();
		std::vector<uint8_t> GetData();

		void AppendBytes(std::vector<uint8_t> data)
		{
			for each (uint8_t b in data)
			{
				Append(b);
			}
		}

		template<typename T>
		void Append(T data)
		{
			for (size_t i = sizeof(T); i > 0; --i)
			{
				Append(static_cast<uint8_t>(data >> ((i - 1) * 8)));
			}
		}

		template<typename T>
		void Append(T *data, size_t sz)
		{
			for (size_t i = 0; i < sz; ++i)
			{
				Append(data[i]);
			}
		}

	private:
		static const size_t defaultCapacity = 128;
		uint8_t *data;
		size_t cp;
		size_t sz;
		void increaseCapacity();
	};
}

#endif // BINPP_BINARYWRITER