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
	DEBUG("Trying to patch: 0x" << std::hex << (DWORD64)patchInfo.address << ":" << patchInfo.length);
	// PROCESS_QUERY_INFORMATION required for VirtualQueryEx
	// PROCESS_VM_OPERATION required for VirtualProtectEx
	// PROCESS_VM_WRITE required for WriteProcessMemory
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, this->processId);
	if (processHandle == NULL) {
		DEBUG("OpenProcess failed: " << std::dec << GetLastError());
		return false;
	}
	DEBUG("processHandle: 0x" << std::hex << processHandle);
	// Get basic information about module memory
	// Especially the region size
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(processHandle, (LPCVOID)patchInfo.address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
		DEBUG("VirtualQueryEx failed: " << std::dec << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG("BaseAddress: 0x" << std::hex << mbi.BaseAddress);
	DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
	// Get system information, like the page size
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DEBUG("PageSize: 0x" << std::hex << si.dwPageSize);
	// Change memory access rights of involved regions
	std::unordered_map<BYTE*, DWORD> oldProtections;
	DWORD oldProtect;
	for (BYTE* baseAddress = (BYTE*)mbi.BaseAddress; baseAddress < (patchInfo.address + patchInfo.length); baseAddress += si.dwPageSize) {
		if (!VirtualProtectEx(processHandle, baseAddress, si.dwPageSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG("VirtualProtectEx failed: " << std::dec << GetLastError());
			DEBUG("baseAddress: 0x" << std::hex << baseAddress);
			DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
			DEBUG("newProtect: 0x" << std::hex << PAGE_EXECUTE_READWRITE);
			DEBUG("oldProtect: 0x" << std::hex << oldProtect);
			CloseHandle(processHandle);
			return false;
		}
		oldProtections[baseAddress] = oldProtect;
	}
	// Patch target memory
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(processHandle, patchInfo.address, patchInfo.opcodes, patchInfo.length, &bytesWritten)) {
		DEBUG("WriteProcessMemory failed: " << std::dec << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG("bytesWritten: " << bytesWritten);
	if (bytesWritten != patchInfo.length) {
		DEBUG("bytesWritten does not match patch length");
		CloseHandle(processHandle);
		return false;
	}
	// Reset memory access rights
	DWORD oldProtect2;
	for (const auto& oldProtection : oldProtections) {
		if (!VirtualProtectEx(processHandle, oldProtection.first, si.dwPageSize, oldProtection.second, &oldProtect2)) {
			DEBUG("VirtualProtectEx failed: " << std::dec << GetLastError());
			DEBUG("baseAddress: 0x" << std::hex << oldProtection.first);
			DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
			DEBUG("newProtect: 0x" << std::hex << oldProtection.second);
			DEBUG("oldProtect: 0x" << std::hex << oldProtect2);
			CloseHandle(processHandle);
			return false;
		}
		//DEBUG("oldProtect: 0x" << std::hex << oldProtection.first << " [0x" << std::hex << oldProtect2 << " => 0x" << std::hex << oldProtection.second << "]");
	}
	CloseHandle(processHandle);
	return true;
}

bool MemoryManager::FindPattern(const std::string& pattern, const void* address, const SIZE_T& size, DWORD& offset) 
{
	// PROCESS_QUERY_INFORMATION required for VirtualQueryEx
	// PROCESS_VM_OPERATION required for VirtualProtectEx
	// PROCESS_VM_WRITE required for WriteProcessMemory
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ, false, this->processId);
	if (processHandle == NULL) {
		DEBUG("OpenProcess failed: " << std::dec << GetLastError());
		return false;
	}
	DEBUG("processHandle: 0x" << std::hex << processHandle);

	// Get basic information about module memory
	// Especially the region size
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(processHandle, (LPCVOID)address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
		DEBUG("VirtualQueryEx failed: " << std::dec << GetLastError());
		CloseHandle(processHandle);
		return false;
	}
	DEBUG("BaseAddress: 0x" << std::hex << mbi.BaseAddress);
	DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
	// Get system information, like the page size
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DEBUG("PageSize: 0x" << std::hex << si.dwPageSize);
	// Change memory access rights of involved regions
	std::unordered_map<DWORD64, DWORD> oldProtections;
	DWORD oldProtect;
	for (DWORD64 baseAddress = (DWORD64)mbi.BaseAddress; baseAddress < ((DWORD64)address + size); baseAddress += si.dwPageSize) {
		if (!VirtualProtectEx(processHandle, (LPVOID)baseAddress, si.dwPageSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG("VirtualProtectEx failed: " << std::dec << GetLastError());
			DEBUG("baseAddress: 0x" << std::hex << baseAddress);
			DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
			DEBUG("newProtect: 0x" << std::hex << PAGE_EXECUTE_READWRITE);
			DEBUG("oldProtect: 0x" << std::hex << oldProtect);
			CloseHandle(processHandle);
			return false;
		}
		oldProtections[baseAddress] = oldProtect;
	}
	
	// Map target memory into local memory
	BYTE* buffer = (BYTE*)malloc(size);
	if (buffer == NULL) {
		DEBUG("malloc failed: " << std::dec << errno);
		CloseHandle(processHandle);
		return false;
	}
	SIZE_T bytesRead = 0;
	if (!ReadProcessMemory(processHandle, (LPCVOID)address, (LPVOID)buffer, size, &bytesRead)) {
		DEBUG("ReadProcessMemory failed: " << std::dec << GetLastError());
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}
	DEBUG("bytesRead: " << bytesRead);
	if (bytesRead != size) {
		DEBUG("bytesRead does not match module size");
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}

	// Reset memory access rights
	DWORD oldProtect2;
	for (const auto& oldProtection : oldProtections) {
		if (!VirtualProtectEx(processHandle, (LPVOID)oldProtection.first, si.dwPageSize, oldProtection.second, &oldProtect2)) {
			DEBUG("VirtualProtectEx failed: " << std::dec << GetLastError());
			DEBUG("baseAddress: 0x" << std::hex << oldProtection.first);
			DEBUG("RegionSize: 0x" << std::hex << mbi.RegionSize);
			DEBUG("newProtect: 0x" << std::hex << oldProtection.second);
			DEBUG("oldProtect: 0x" << std::hex << oldProtect2);
			CloseHandle(processHandle);
			return false;
		}
	}

	// Parse pattern into sig and mask
	DEBUG("pattern: " << pattern);
	std::vector<BYTE> vSig;
	std::vector<char> vMask;
	std::istringstream iss(pattern);
	for (std::string token; std::getline(iss, token, ' '); )
	{
		if (!token.compare("?") || !token.compare("??")) {
			vSig.push_back(0x0);
			vMask.push_back('?');
		}
		else
		{
			BYTE b = (BYTE)std::strtoul(token.c_str(), nullptr, 16);
			vSig.push_back(b);
			vMask.push_back('x');
		}
	}
	vMask.push_back('\0');

	// Search local memory for pattern
	offset = NULL;
	for (SIZE_T tmp = 0; tmp < size; ++tmp) {
		if (this->CompareData(buffer + tmp, &vSig[0], &vMask[0])) {
			offset = (DWORD)tmp;
			break;
		}
	}
	DEBUG("offset: 0x" << std::hex << offset);
	if (offset == NULL) {
		DEBUG("Pattern not found");
		free(buffer);
		CloseHandle(processHandle);
		return false;
	}
	free(buffer);
	CloseHandle(processHandle);
	return true;
}

bool MemoryManager::CompareData(const BYTE* data, const BYTE* sig, const char* mask) 
{
	for (auto i = 0; mask[i]; ++i) {
		if (mask[i] != '?' && sig[i] != data[i]) {
			return false;
		}
	}
	return true;
}