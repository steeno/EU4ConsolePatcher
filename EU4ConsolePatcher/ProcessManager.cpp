#include "stdafx.h"
#include "ProcessManager.h"

ProcessManager::ProcessManager() {
}

ProcessManager::~ProcessManager() {
}

bool ProcessManager::FindProcess(const wchar_t* processName, PROCESSENTRY32& pe32) {
	if (!this->UpdateProcessList()) {
		return false;
	}
	for (auto it = this->processList.cbegin(); it != this->processList.cend(); ++it) {
		if (!lstrcmp(processName, it->szExeFile)) {
			pe32 = *it;
			return true;
		}
	}
	return false;
}

bool ProcessManager::UpdateProcessList() {
	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (processSnapshot == INVALID_HANDLE_VALUE) {
		std::wcerr << "CreateToolhelp32Snapshot failed: " << GetLastError() << std::endl;
		return false;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(processSnapshot, &pe32)) {
		std::wcerr << "Process32First failed: " << GetLastError() << std::endl;
		return false;
	}
	do {
		this->processList.push_back(pe32);
	} while (Process32Next(processSnapshot, &pe32));
	return true;
}