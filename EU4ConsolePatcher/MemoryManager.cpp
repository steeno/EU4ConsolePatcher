#include "stdafx.h"
#include "MemoryManager.h"
#include "structs.h"

MemoryManager::MemoryManager(const DWORD& processId)
{
	this->processId = processId;
}

MemoryManager::~MemoryManager()
{
}

bool MemoryManager::Patch(const patchInfo_t& patchInfo) {
	HANDLE processHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, false, this->processId);
	if (processHandle == NULL) {
		DEBUG(L"OpenProcess failed: " << GetLastError());
		return false;
	}
	DWORD oldProtect;
	LPVOID address = (void*)(patchInfo.baseAddress + patchInfo.offset);
	DEBUG(L"address: " << std::hex << address);
	if (!VirtualProtectEx(processHandle, address, patchInfo.length, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		DEBUG(L"VirtualProtectEx failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"oldProtect: " << std::hex << oldProtect);
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(processHandle, address, patchInfo.opcodes, patchInfo.length, &bytesWritten)) {
		DEBUG(L"WriteProcessMemory failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"bytesWritten: " << bytesWritten);
	if (bytesWritten != patchInfo.length) {
		DEBUG(L"bytesWritten does not match patch length");
		CloseHandle(processHandle);
		return false;
	}
	DWORD oldProtect2;
	if (!VirtualProtectEx(processHandle, address, patchInfo.length, oldProtect, &oldProtect2)) {
		DEBUG(L"VirtualProtectEx failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	CloseHandle(processHandle);
	return true;
}
