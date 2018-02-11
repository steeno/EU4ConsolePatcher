#pragma once
class MemoryManager
{
public:
	MemoryManager(const DWORD& processId);
	~MemoryManager();
	bool Patch(const DWORD& baseAddress, const BYTE* buffer, const SIZE_T& length);
private:
	DWORD processId;
};

