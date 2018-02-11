#include "stdafx.h"
#include "MemoryManager.h"

MemoryManager::MemoryManager(const DWORD& processId)
{
	this->processId = processId;
}

MemoryManager::~MemoryManager()
{
}

bool MemoryManager::Patch(const DWORD& baseAddress, const BYTE* buffer, const SIZE_T& length) {
	HANDLE processHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, false, this->processId);
	if (processHandle == NULL) {
		std::wcerr << L"OpenProcess failed: " << GetLastError() << std::endl;
		return false;
	}
	DWORD oldProtect;
	if (!VirtualProtectEx(processHandle, (void*)baseAddress, length, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		std::wcerr << L"VirtualProtectEx failed: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return false;
	}
	std::wcout << L"oldProtect: " << std::hex << oldProtect << std::endl;
	BYTE* oldBuffer = new BYTE[length];
	SIZE_T bytesRead;
	if (!ReadProcessMemory(processHandle, (const void*)baseAddress, (void*)oldBuffer, length, &bytesRead)) {
		std::wcerr << L"ReadProcessMemory failed: " << GetLastError() << std::endl;
		delete[] oldBuffer;
		CloseHandle(processHandle);
		return false;
	}
	std::wcout << L"bytesRead: " << std::dec << bytesRead << std::endl << \
		L"oldBuffer: " << std::hex << oldBuffer[0] << std::endl;
	delete[] oldBuffer;
	/*
	TODO
	- compare oldBuffer with expected opcodes
	- compare bytesRead with length
	*/
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(processHandle, (void*)baseAddress, buffer, length, &bytesWritten)) {
		std::wcerr << L"WriteProcessMemory failed: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return false;
	}
	std::wcout << L"bytesWritten: " << std::dec << bytesWritten << std::endl << \
		L"buffer: " << std::hex << buffer[0] << std::endl;
	/* 
	TODO 
	- compare bytesWritten with length
	*/
	DWORD oldProtect2;
	if (!VirtualProtectEx(processHandle, (void*)baseAddress, length, oldProtect, &oldProtect2)) {
		std::wcerr << L"VirtualProtectEx failed: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return false;
	}
	CloseHandle(processHandle);
	return true;
}
