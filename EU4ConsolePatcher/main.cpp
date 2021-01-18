#include "stdafx.h"
#include "ProcessManager.h"
#include "ModuleManager.h"
#include "MemoryManager.h"
#include "structs.h"
#include "defines.h"

int main() 
{
	ProcessManager processManager;
	std::cout << "Trying to enable debug privileges for the current process" << std::endl;
	if (!processManager.GrantDebugPrivileges(GetCurrentProcess())) {
		std::cout << "Could not enable debug privileges" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::cout << "Debug privileges enabled" << std::endl;
	PROCESSENTRY32 pe32;
	std::cout << "Trying to find process " << TARGET_PROCESS_NAME << std::endl;
	if (!processManager.FindProcess(TARGET_PROCESS_NAME, pe32)) {
		std::cout << TARGET_PROCESS_NAME << " process could not be found, is it already running?" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::cout << TARGET_PROCESS_NAME << " process found" << std::endl;

	ModuleManager moduleManager(pe32.th32ProcessID);
	MODULEENTRY32 me32;
	std::cout << "Trying to find module " << TARGET_MODULE_NAME << std::endl;
	if (!moduleManager.FindModule(TARGET_MODULE_NAME, me32)) {
		std::cout << TARGET_MODULE_NAME << " module could not be found, has the target app completely loaded?" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::cout << TARGET_MODULE_NAME << " module found" << std::endl;

	MemoryManager memoryManager(pe32.th32ProcessID);
	std::vector<patchInfo_t> patches;
	DWORD offset;
	patchInfo_t patch;
	std::cout << "Trying to find memory patterns" << std::endl;
	if (!memoryManager.FindPattern(MEMORY_PATTERN_A, me32.modBaseAddr, me32.modBaseSize, offset)) {
		std::cout << "First memory pattern could not be found" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::cout << "First memory pattern found" << std::endl;
	patch.address = me32.modBaseAddr + offset;
	patch.opcodes = new BYTE {
		OPCODE_JMP
	};
	patch.length = 1;
	patches.push_back(patch);
	if (!memoryManager.FindPattern(MEMORY_PATTERN_B, me32.modBaseAddr, me32.modBaseSize, offset)) {
		std::cout << "Second memory pattern could not be found" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::cout << "Second memory pattern found" << std::endl;
	patch.address = me32.modBaseAddr + offset;
	patch.opcodes = new BYTE {
		OPCODE_JMP
	};
	patch.length = 1;
	patches.push_back(patch);

	std::cout << "Trying to apply patches" << std::endl;
	for (auto p = patches.cbegin(); p != patches.cend(); ++p) {
		if (!memoryManager.Patch(*p)) {
			std::cout << "Patch could not be applied" << std::endl;
			system("pause");
			return EXIT_FAILURE;
		}
		std::cout << "Patch applied successfully" << std::endl;
	}

#ifdef _DEBUG
	system("pause");
#endif
	return EXIT_SUCCESS;
}

