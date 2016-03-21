#include "io.hpp"

namespace io
{
	bool directory::exist(std::string path)
	{
		DWORD ftyp = GetFileAttributesA(path.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
		if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
		return false;
	}

	bool directory::create(std::string path)
	{
		BOOL res = CreateDirectoryA(path.c_str(), NULL);

		if (res == NULL)
		{
			DWORD err = GetLastError();
			if (err == ERROR_ALREADY_EXISTS)
			{
				return true;
			}
			
			return false;
		}

		return true;
	}

	std::vector<std::wstring> file::list(std::wstring directory)
	{
		WIN32_FIND_DATAW ffd;
		HANDLE hFind;

		std::vector<std::wstring> result;

		if (directory.length() > (MAX_PATH - 3))
		{
			return result;
		}

		directory.append(L"\\*");

		// Find the first file in the directory
		hFind = FindFirstFileW(directory.c_str(), &ffd);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return result;
		}

		// List all the files in the directory
		do
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				result.push_back(ffd.cFileName);
			}
		} while (FindNextFileW(hFind, &ffd) != 0);

		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
		{
			return result;
		}

		FindClose(hFind);
		return result;
	}
}