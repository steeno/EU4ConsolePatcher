#pragma once
#include "structs.h"

class MemoryManager
{
public:
	MemoryManager(const DWORD& processId);
	~MemoryManager();
	bool Patch(const patchInfo_t& patchInfo);
	bool FindPattern(const wchar_t* pattern, const wchar_t* signature, const BYTE* address, const SIZE_T& size, DWORD& offset);
private:
	DWORD processId;

	bool CompareData(const BYTE* data, const wchar_t* pattern, const wchar_t* signature);
};

