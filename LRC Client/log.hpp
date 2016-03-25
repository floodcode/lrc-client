#pragma once

#include <string>

using std::string;
using std::wstring;

namespace Log
{
	void Info(const string &message);
	void Info(const wstring &message);
	void Error(const string &message);
	void Error(const wstring &message);
}