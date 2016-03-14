#include "hwinfo.hpp"
#include "winfx.hpp"

hwinfo::CPUID hwinfo::GetCPUID()
{
	char cpu_name_string[49] = { 0 };
	char cpu_vendor_id_string[13] = { 0 };
	unsigned int cpu_feat_eax = 0;
	unsigned int cpu_feat_edx = 0;
	unsigned int cpu_feat_ecx = 0;
	unsigned int cpu_feat_ext_edx = 0;

	__asm
	{
		mov     eax, 0x00000000
		cpuid
		mov     DWORD PTR[cpu_vendor_id_string + 0], ebx
		mov     DWORD PTR[cpu_vendor_id_string + 4], edx
		mov     DWORD PTR[cpu_vendor_id_string + 8], ecx
		test    eax, eax
		jz      no_features

		mov     eax, 0x00000001
		cpuid
		mov[cpu_feat_eax], eax
		mov[cpu_feat_edx], edx
		mov[cpu_feat_ecx], ecx

		mov     eax, 0x80000000
		cpuid
		cmp     eax, 0x80000001
		jb      no_features
		cmp     eax, 0x80000004
		jb      ext_feats_only


		mov     eax, 0x80000002
		cpuid
		mov     DWORD PTR[cpu_name_string + 0], eax
		mov     DWORD PTR[cpu_name_string + 4], ebx
		mov     DWORD PTR[cpu_name_string + 8], ecx
		mov     DWORD PTR[cpu_name_string + 12], edx

		mov     eax, 0x80000003
		cpuid
		mov     DWORD PTR[cpu_name_string + 16], eax
		mov     DWORD PTR[cpu_name_string + 20], ebx
		mov     DWORD PTR[cpu_name_string + 24], ecx
		mov     DWORD PTR[cpu_name_string + 28], edx

		mov     eax, 0x80000004
		cpuid
		mov     DWORD PTR[cpu_name_string + 32], eax
		mov     DWORD PTR[cpu_name_string + 36], ebx
		mov     DWORD PTR[cpu_name_string + 40], ecx
		mov     DWORD PTR[cpu_name_string + 44], edx

	ext_feats_only:
		mov     eax, 0x80000001
		cpuid
		mov[cpu_feat_ext_edx], edx

	no_features:
	}

	CPUID result;

	result.cpuName = std::string(cpu_name_string);
	result.cpuVendor = std::string(cpu_vendor_id_string);

	result.stepping = cpu_feat_eax & 0xF;
	result.model = (cpu_feat_eax >> 4) & 0xF;
	result.family = (cpu_feat_eax >> 8) & 0xF;
	result.type = (cpu_feat_eax >> 12) & 0x3;
	result.extModel = (cpu_feat_eax >> 16) & 0xF;
	result.extFamily = (cpu_feat_eax >> 20) & 0xFF;

	result.hasMMX = (cpu_feat_edx >> 23) & 0x1;
	result.hasSSE = (cpu_feat_edx >> 25) & 0x1;
	result.hasSSE2 = (cpu_feat_edx >> 26) & 0x1;
	result.isHTT = (cpu_feat_edx >> 28) & 0x1;

	result.hasSSE3 = cpu_feat_ecx & 0x1;

	result.hasMMXExt = (cpu_feat_ext_edx >> 22) & 0x1;
	result.has3DNow = (cpu_feat_ext_edx >> 31) & 0x1;
	result.has3DNowExt = (cpu_feat_ext_edx >> 30) & 0x1;

	return result;
}


hwinfo::SystemInfo hwinfo::GetSystemInfo()
{
	SYSTEM_INFO winSystemInfo;
	::GetSystemInfo(&winSystemInfo);

	return SystemInfo();
}