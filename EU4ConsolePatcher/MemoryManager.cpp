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
	// PROCESS_QUERY_INFORMATION required for VirtualQueryEx
	// PROCESS_VM_OPERATION required for VirtualProtectEx
	// PROCESS_VM_WRITE required for WriteProcessMemory
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, this->processId);
	if (processHandle == NULL) {
		DEBUG(L"OpenProcess failed: " << GetLastError());
		return false;
	}
	// Get basic information about module memory
	// Especially the region size
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(processHandle, (LPCVOID)patchInfo.address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
		DEBUG(L"VirtualQueryEx failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"RegionSize: 0x" << std::hex << mbi.RegionSize);
	std::unordered_map<BYTE*, DWORD> oldProtections;
	DWORD oldProtect;
	for (BYTE* baseAddress = (BYTE*)mbi.BaseAddress; baseAddress < (patchInfo.address + patchInfo.length); baseAddress += mbi.RegionSize) {
		if (!VirtualProtectEx(processHandle, baseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		DEBUG(L"oldProtect: 0x" << std::hex << baseAddress << " [0x" << std::hex << oldProtect << " => " << PAGE_EXECUTE_READWRITE << "]");
		oldProtections[baseAddress] = oldProtect;
	}
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(processHandle, patchInfo.address, patchInfo.opcodes, patchInfo.length, &bytesWritten)) {
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
	for (const auto& oldProtection : oldProtections) {
		if (!VirtualProtectEx(processHandle, oldProtection.first, mbi.RegionSize, oldProtection.second, &oldProtect2)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		DEBUG(L"oldProtect: 0x" << std::hex << oldProtection.first << " [0x" << std::hex << oldProtect2 << " => " << oldProtection.second << "]");
	}
	CloseHandle(processHandle);
	return true;
}
