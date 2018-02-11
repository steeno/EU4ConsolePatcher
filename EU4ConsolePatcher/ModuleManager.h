#pragma once

#include "stdafx.h"

class ModuleManager
{
public:
	ModuleManager(const DWORD& processId);
	~ModuleManager();
	bool FindModule(const wchar_t* moduleName, MODULEENTRY32& me32);
private:
	DWORD processId;
	std::vector<MODULEENTRY32> moduleList;
	bool UpdateModuleList();
};

