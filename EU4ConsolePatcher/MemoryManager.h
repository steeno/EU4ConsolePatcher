#pragma once
#include "structs.h"

class MemoryManager
{
public:
	MemoryManager(const DWORD& processId);
	~MemoryManager();
	bool Patch(const patchInfo_t& patchInfo);
	bool FindPattern(const std::string& pattern, const void* address, const SIZE_T& size, DWORD& offset);
	
private:
	DWORD processId;

	bool CompareData(const BYTE* data, const BYTE* sig, const char* mask);
};

