#pragma once
#include "structs.h"

class MemoryManager
{
public:
	MemoryManager(const DWORD& processId);
	~MemoryManager();
	bool Patch(const patchInfo_t& patchInfo);
private:
	DWORD processId;
};

