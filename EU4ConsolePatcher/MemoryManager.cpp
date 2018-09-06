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

bool MemoryManager::Patch(const patchInfo_t& patchInfo) 
{
	DEBUG(L"Trying to patch: " << std::hex << patchInfo.address << ":" << patchInfo.length);
	// PROCESS_QUERY_INFORMATION required for VirtualQueryEx
	// PROCESS_VM_OPERATION required for VirtualProtectEx
	// PROCESS_VM_WRITE required for WriteProcessMemory
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, this->processId);
	if (processHandle == NULL) {
		DEBUG(L"OpenProcess failed: " << GetLastError());
		return false;
	}
	DEBUG(L"processHandle: 0x" << std::hex << processHandle);
	// Get basic information about module memory
	// Especially the region size
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(processHandle, (LPCVOID)patchInfo.address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
		DEBUG(L"VirtualQueryEx failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"RegionSize: 0x" << std::hex << mbi.RegionSize);
	// Change memory access rights of involved regions
	std::unordered_map<BYTE*, DWORD> oldProtections;
	DWORD oldProtect;
	for (BYTE* baseAddress = (BYTE*)mbi.BaseAddress; baseAddress < (patchInfo.address + patchInfo.length); baseAddress += mbi.RegionSize) {
		if (!VirtualProtectEx(processHandle, baseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		//DEBUG(L"oldProtect: 0x" << std::hex << baseAddress << " [0x" << std::hex << oldProtect << " => 0x" << std::hex << PAGE_EXECUTE_READWRITE << "]");
		oldProtections[baseAddress] = oldProtect;
	}
	// Patch target memory
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
	// Reset memory access rights
	DWORD oldProtect2;
	for (const auto& oldProtection : oldProtections) {
		if (!VirtualProtectEx(processHandle, oldProtection.first, mbi.RegionSize, oldProtection.second, &oldProtect2)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		//DEBUG(L"oldProtect: 0x" << std::hex << oldProtection.first << " [0x" << std::hex << oldProtect2 << " => 0x" << std::hex << oldProtection.second << "]");
	}
	CloseHandle(processHandle);
	return true;
}

bool MemoryManager::FindPattern(const wchar_t* pattern, const BYTE* address, const SIZE_T& size, DWORD& offset) 
{
	// PROCESS_QUERY_INFORMATION required for VirtualQueryEx
	// PROCESS_VM_OPERATION required for VirtualProtectEx
	// PROCESS_VM_READ required for ReadProcessMemory
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ, false, this->processId);
	if (processHandle == NULL) {
		DEBUG(L"OpenProcess failed: " << GetLastError());
		return false;
	}
	DEBUG(L"processHandle: 0x" << std::hex << processHandle);
	// Get basic information about module memory
	// Especially the region size
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(processHandle, (LPCVOID)address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
		DEBUG(L"VirtualQueryEx failed: " << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"RegionSize: 0x" << std::hex << mbi.RegionSize);
	// Change memory access rights of involved regions
	std::unordered_map<BYTE*, DWORD> oldProtections;
	DWORD oldProtect;
	for (auto baseAddress = (BYTE*)mbi.BaseAddress; baseAddress < (address + size); baseAddress += mbi.RegionSize) {
		if (!VirtualProtectEx(processHandle, baseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		//DEBUG(L"oldProtect: 0x" << std::hex << baseAddress << " [0x" << std::hex << oldProtect << " => 0x" << std::hex << PAGE_EXECUTE_READWRITE << "]");
		oldProtections[baseAddress] = oldProtect;
	}
	// Map target memory into local memory
	BYTE* buffer = (BYTE*)malloc(size);
	SIZE_T bytesRead = 0;
	if (!ReadProcessMemory(processHandle, (LPCVOID)address, (LPVOID)buffer, size, &bytesRead)) {
		DEBUG(L"ReadProcessMemory failed: " << GetLastError());
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}
	DEBUG(L"bytesRead: " << bytesRead);
	if (bytesRead != size) {
		DEBUG(L"bytesRead does not match module size");
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}
	offset = NULL;
	// Search local memory for pattern
	for (SIZE_T tmp = 0; tmp < size; ++tmp) {
		if (this->CompareData(buffer + tmp, pattern)) {
			offset = tmp;
			DEBUG(L"offset: 0x" << std::hex << offset);
			break;
		}
	}
	if (offset == NULL) {
		DEBUG(L"Pattern not found");
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}
	free(buffer);
	// Reset memory access rights
	DWORD oldProtect2;
	for (const auto& oldProtection : oldProtections) {
		if (!VirtualProtectEx(processHandle, oldProtection.first, mbi.RegionSize, oldProtection.second, &oldProtect2)) {
			DEBUG(L"VirtualProtectEx failed: " << GetLastError());
			CloseHandle(processHandle);
			return false;
		}
		//DEBUG(L"oldProtect: 0x" << std::hex << oldProtection.first << " [0x" << std::hex << oldProtect2 << " => 0x" << std::hex << oldProtection.second << "]");
	}
	CloseHandle(processHandle);
	return true;	
}

bool MemoryManager::CompareData(const BYTE* data, const wchar_t* pattern) 
{
	for (auto i = 0; pattern[i]; ++i) {
		if (pattern[i] != L'?' && pattern[i] != data[i]) {
			return false;
		}
	}
	return true;
}