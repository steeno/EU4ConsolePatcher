#include "stdafx.h"
#include "ProcessManager.h"
#include "ModuleManager.h"
#include "MemoryManager.h"
#include "structs.h"
#include "defines.h"

int main() 
{
	ProcessManager processManager;
	std::wcout << L"Trying to enable debug privileges for the current process" << std::endl;
	if (!processManager.GrantDebugPrivileges(GetCurrentProcess())) {
		std::wcout << L"Could not enable debug privileges" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	std::wcout << L"Debug privileges enabled" << std::endl;
	PROCESSENTRY32 pe32;
	std::wcout << L"Trying to find process " << TARGET_PROCESS_NAME << std::endl;
	if (!processManager.FindProcess(TARGET_PROCESS_NAME, pe32)) {
		std::wcout << TARGET_PROCESS_NAME << L" process could not be found, is it already running?" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	std::wcout << TARGET_PROCESS_NAME << L" process found" << std::endl;

	ModuleManager moduleManager(pe32.th32ProcessID);
	MODULEENTRY32 me32;
	std::wcout << L"Trying to find module " << TARGET_MODULE_NAME << std::endl;
	if (!moduleManager.FindModule(TARGET_MODULE_NAME, me32)) {
		std::wcout << TARGET_MODULE_NAME << L" module could not be found, has the target app completely loaded?" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	std::wcout << TARGET_MODULE_NAME << L" module found" << std::endl;

	MemoryManager memoryManager(pe32.th32ProcessID);
	std::vector<patchInfo_t> patches;
	DWORD offset;
	patchInfo_t patch;
	std::wcout << L"Trying to find memory patterns" << std::endl;
	if (!memoryManager.FindPattern(MEMORY_PATTERN_A, MEMORY_SIGNATURE_A, me32.modBaseAddr, me32.modBaseSize, offset)) {
		std::wcout << L"First memory pattern could not be found" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	std::wcout << L"First memory pattern found" << std::endl;
	patch.address = me32.modBaseAddr + offset;
	patch.opcodes = new BYTE {
		OPCODE_JMP
	};
	patch.length = 1;
	patches.push_back(patch);
	if (!memoryManager.FindPattern(MEMORY_PATTERN_B, MEMORY_SIGNATURE_B, me32.modBaseAddr, me32.modBaseSize, offset)) {
		std::wcout << L"Second memory pattern could not be found" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	std::wcout << L"Second memory pattern found" << std::endl;
	patch.address = me32.modBaseAddr + offset;
	patch.opcodes = new BYTE {
		OPCODE_JMP
	};
	patch.length = 1;
	patches.push_back(patch);

	std::wcout << L"Trying to apply patches" << std::endl;
	for (auto p = patches.cbegin(); p != patches.cend(); ++p) {
		if (!memoryManager.Patch(*p)) {
			std::wcout << L"Patch could not be applied" << std::endl;
			getchar();
			return EXIT_FAILURE;
		}
		std::wcout << L"Patch applied successfully" << std::endl;
	}

#ifdef _DEBUG
	getchar();
#endif
	return EXIT_SUCCESS;
}

