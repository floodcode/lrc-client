#include "binarywriter.hpp"

using namespace binpp;

BinaryWriter::BinaryWriter()
{
	this->sz = 0;
	this->cp = this->defaultCapacity;
	data = new uint8_t[this->cp];
}

BinaryWriter::BinaryWriter(size_t cp)
{
	this->sz = 0;
	this->cp = cp;
	data = new uint8_t[cp];
}

BinaryWriter::~BinaryWriter()
{
	delete[] data;
}

void BinaryWriter::Append(uint8_t data)
{
	if (this->cp == this->sz)
	{
		increaseCapacity();
	}

	this->data[this->sz] = data;
	this->sz++;
}

void BinaryWriter::AppendBytes(std::vector<uint8_t> data)
{
	for each (uint8_t b in data)
	{
		Append(b);
	}
}

void BinaryWriter::GetBytes(uint8_t *out, size_t count)
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

void BinaryWriter::Clear()
{
	Clear(this->defaultCapacity);
}

void BinaryWriter::Clear(size_t capacity)
{
	delete[] this->data;
	this->cp = capacity;
	this->sz = 0;
	data = new uint8_t[this->cp];
}

bool BinaryWriter::Save(std::string filename)
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

size_t BinaryWriter::Size()
{
	return this->sz;
}

size_t BinaryWriter::Capacity()
{
	return this->cp;
}

uint8_t *BinaryWriter::Data()
{
	return this->data;
}

std::vector<uint8_t> BinaryWriter::GetData()
{
	std::vector<uint8_t> result;
	result.reserve(this->sz);

	for (size_t i = 0; i < sz; ++i)
	{
		result.push_back(data[i]);
	}

	return result;
}

void BinaryWriter::increaseCapacity()
{
	uint8_t *oldData = data;
	data = new uint8_t[this->cp * 2];
	for (size_t i = 0; i < this->cp; ++i)
	{
		data[i] = oldData[i];
	}
	delete[] oldData;
	cp *= 2;
}