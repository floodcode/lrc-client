#include "io.h"

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