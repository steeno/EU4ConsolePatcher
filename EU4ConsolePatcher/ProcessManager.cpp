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
		DEBUG(L"process name: " << it->szExeFile);
		if (!lstrcmp(processName, it->szExeFile)) {
			pe32 = *it;
			return true;
		}
	}
	return false;
}

bool ProcessManager::GrantDebugPrivileges(const HANDLE& processHandle) {
	HANDLE accessTokenHandle;
	if (!OpenProcessToken(processHandle, TOKEN_ADJUST_PRIVILEGES, &accessTokenHandle)) {
		DEBUG(L"OpenProcessToken failed: " << GetLastError());
		return false;
	}
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		DEBUG(L"LookupPrivilegeValue failed: " << GetLastError());
		CloseHandle(accessTokenHandle);
		return false;
	}
	TOKEN_PRIVILEGES tokenPrivileges;
	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(accessTokenHandle, false, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		DEBUG(L"AdjustTokenProvileges failed: " << GetLastError());
		CloseHandle(accessTokenHandle);
		return false;
	}
	CloseHandle(accessTokenHandle);
	return true;
}

bool ProcessManager::UpdateProcessList() {
	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (processSnapshot == INVALID_HANDLE_VALUE) {
		DEBUG(L"CreateToolhelp32Snapshot failed: " << GetLastError());
		return false;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(processSnapshot, &pe32)) {
		DEBUG(L"Process32First failed: " << GetLastError());
		return false;
	}
	do {
		this->processList.push_back(pe32);
	} while (Process32Next(processSnapshot, &pe32));
	return true;
}