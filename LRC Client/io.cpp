#include "io.hpp"

bool io::directory::exist(std::string path)
{
	DWORD ftyp = GetFileAttributesA(path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
	return false;
}

bool io::directory::create(std::string path)
{
	return CreateDirectoryA(path.c_str(), NULL) != FALSE;
}

bool io::file::write(std::string path, std::string text)
{
	std::ofstream output(path, std::ios::trunc);
	if (output.is_open())
	{
		output << text;
		output.close();
		return true;
	}
	return false;
}