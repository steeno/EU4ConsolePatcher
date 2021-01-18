#include "stdafx.h"
#include "ModuleManager.h"
#include "defines.h"

ModuleManager::ModuleManager(const DWORD& processId)
{
	this->processId = processId;
}

ModuleManager::~ModuleManager()
{
}

bool ModuleManager::FindModule(const std::string& moduleName, MODULEENTRY32& me32) 
{
	if (!this->UpdateModuleList()) {
		return false;
	}
	for (auto it = this->moduleList.cbegin(); it != this->moduleList.cend(); ++it) {
		DEBUG("module name: " << it->szModule);
		if (!moduleName.compare(it->szModule)) {
			me32 = *it;
			DEBUG("modBaseAddress: 0x" << std::hex << (DWORD64)me32.modBaseAddr);
			DEBUG("modBaseSize: 0x" << std::hex << me32.modBaseSize);
			return true;
		}
	}
	return false;
}

bool ModuleManager::UpdateModuleList() 
{
	HANDLE moduleSnapshot;
	DWORD lastError;
	BYTE retries = 0;

	while (true) {
		moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->processId);
		if (moduleSnapshot != INVALID_HANDLE_VALUE) {
			break;
		}
		// If CTH32S failed with ERROR_PARTIAL_COPY error, try again
		lastError = GetLastError();
		if (lastError == ERROR_PARTIAL_COPY) {
			++retries;
			// If CTH32S failed to often with ERROR_PARTIAL_COPY error, cancel
			if (retries > MAX_MODULE_SNAPSHOT_RETRY_COUNT) {
				DEBUG("CreateToolhelp32Snapshot failed: " << lastError);
				return false;
			}
			Sleep(1000);
			continue;
		}
		// IF CTH32S failed with another error, cancel
		DEBUG("CreateToolhelp32Snapshot failed: " << lastError);
		return false;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(moduleSnapshot, &me32)) {
		DEBUG("Module32First failed: " << GetLastError());
		CloseHandle(moduleSnapshot);
		return false;
	}
	do {
		this->moduleList.push_back(me32);
	} while (Module32Next(moduleSnapshot, &me32));
	CloseHandle(moduleSnapshot);
	return true;
}
