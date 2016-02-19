#ifndef BINPP_BINARYWRITER
#define BINPP_BINARYWRITER
#include "binpptypes.hpp"
#include <string>
#include <fstream>

namespace binpp
{
	class BinaryWriter
	{
	public:
		BinaryWriter();
		BinaryWriter(size_t cp);
		~BinaryWriter();
		void Append(byte data);
		void GetBytes(byte *out, size_t count);
		void Clear();
		void Clear(size_t capacity);
		bool Save(std::string filename);
		size_t Size();
		size_t Capacity();

		template<typename T>
		void Append(T data)
		{
			for (size_t i = sizeof(T); i > 0; --i)
			{
				Append(static_cast<byte>(data >> ((i - 1) * 8)));
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
		byte *data;
		size_t cp;
		size_t sz;
		void increaseCapacity();
	};
}

#endif // BINPP_BINARYWRITER