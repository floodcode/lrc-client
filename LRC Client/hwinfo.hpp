#pragma once
#include <string>

namespace hwinfo
{
	using namespace std;

	struct CPUID
	{
		bool hasMMX;
		bool hasMMXExt;
		bool has3DNow;
		bool has3DNowExt;
		bool hasSSE;
		bool hasSSE2;
		bool hasSSE3;
		bool isHTT;

		int stepping;
		int model;
		int family;
		int type;
		int extModel;
		int extFamily;

		string cpuName;
		string cpuVendor;

		unsigned long cpuSpeed;
	};

	struct SystemInfo
	{
		string OSVersion;
		string cpu;
		uint32_t cores;
		uint32_t ram;
	};

	CPUID GetCPUID();
	SystemInfo GetSystemInfo();
}