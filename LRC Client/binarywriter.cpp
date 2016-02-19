#include "binarywriter.hpp"

binpp::BinaryWriter::BinaryWriter()
{
	this->sz = 0;
	this->cp = this->defaultCapacity;
	data = new byte[this->cp];
}

binpp::BinaryWriter::BinaryWriter(size_t cp)
{
	this->sz = 0;
	this->cp = cp;
	data = new byte[cp];
}

binpp::BinaryWriter::~BinaryWriter()
{
	delete[] data;
}

void binpp::BinaryWriter::Append(byte data)
{
	if (this->cp == this->sz)
	{
		increaseCapacity();
	}

	this->data[this->sz] = data;
	this->sz++;
}

void binpp::BinaryWriter::GetBytes(byte *out, size_t count)
{
	if (count > this->sz)
	{
		throw "Requested bytes count shold be <= size()";
		return;
	}

	for (size_t i = 0; i < count; i++)
	{
		out[i] = this->data[i];
	}
}

void binpp::BinaryWriter::Clear()
{
	Clear(this->defaultCapacity);
}

void binpp::BinaryWriter::Clear(size_t capacity)
{
	delete[] this->data;
	this->cp = capacity;
	this->sz = 0;
	data = new byte[this->cp];
}

bool binpp::BinaryWriter::Save(std::string filename)
{
	std::ofstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!output.is_open())
	{
		return false;
	}
	else
	{
		output.write((char *)this->data, this->sz);
		output.close();
		return true;
	}
}

size_t binpp::BinaryWriter::Size()
{
	return this->sz;
}

size_t binpp::BinaryWriter::Capacity()
{
	return this->cp;
}

void binpp::BinaryWriter::increaseCapacity()
{
	byte *oldData = data;
	data = new byte[this->cp * 2];
	for (size_t i = 0; i < this->cp; ++i)
	{
		data[i] = oldData[i];
	}
	delete[] oldData;
	cp *= 2;
}