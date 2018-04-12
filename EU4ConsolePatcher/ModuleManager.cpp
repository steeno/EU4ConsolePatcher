#include "stdafx.h"
#include "ModuleManager.h"

ModuleManager::ModuleManager(const DWORD& processId)
{
	this->processId = processId;
}

ModuleManager::~ModuleManager()
{
}

bool ModuleManager::FindModule(const wchar_t* moduleName, MODULEENTRY32& me32) 
{
	if (!this->UpdateModuleList()) {
		return false;
	}
	for (auto it = this->moduleList.cbegin(); it != this->moduleList.cend(); ++it) {
		DEBUG(L"module name: " << it->szModule);
		if (!lstrcmp(moduleName, it->szModule)) {
			me32 = *it;
			return true;
		}
	}
	return false;
}

bool ModuleManager::UpdateModuleList() 
{
	HANDLE moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->processId);
	if (moduleSnapshot == INVALID_HANDLE_VALUE) {
		DEBUG(L"CreateToolhelp32Snapshot failed: " << GetLastError());
		return false;
	}
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(moduleSnapshot, &me32)) {
		DEBUG(L"Module32First failed: " << GetLastError());
		CloseHandle(moduleSnapshot);
		return false;
	}
	do {
		this->moduleList.push_back(me32);
	} while (Module32Next(moduleSnapshot, &me32));
	CloseHandle(moduleSnapshot);
	return true;
}
