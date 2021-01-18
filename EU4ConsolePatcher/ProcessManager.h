#pragma once

class ProcessManager
{
public:
	ProcessManager();
	~ProcessManager();
	bool FindProcess(const std::string& processName, PROCESSENTRY32& pe32);
	bool GrantDebugPrivileges(const HANDLE& processHandle);
private:
	std::vector<PROCESSENTRY32> processList;
	bool UpdateProcessList();
};

