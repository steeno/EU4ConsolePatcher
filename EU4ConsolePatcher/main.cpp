#include "stdafx.h"
#include "ProcessManager.h"
#include "ModuleManager.h"
#include "MemoryManager.h"
#include "structs.h"
#include "defines.h"

/* v1.24.1 
>eu4.exe
018677C1 | E977C1 | 74 18                    | je eu4.18677DB                          |
018677C3 | E977C3 | 6A 35                    | push 35                                 |
018677C5 | E977C5 | 68 20 57 DA 01           | push eu4.1DA5720                        | 1DA5720:"Command not available in multiplayer or ironman mode."
018677CA | E977CA | 8D 4D D8                 | lea ecx,dword ptr ss:[ebp-28]           |
018677CD | E977CD | C6 45 D4 00              | mov byte ptr ss:[ebp-2C],0              |
018677D1 | E977D1 | E8 BA 27 26 FF           | call eu4.AC9F90                         |
018677D6 | E977D6 | E9 58 03 00 00           | jmp eu4.1867B33                         |
018677DB | E977DB | 8B 45 D0                 | mov eax,dword ptr ss:[ebp-30]           |

01867990 | E97990 | 74 0A                    | je eu4.186799C                          |
01867992 | E97992 | C7 05 00 00 00 00 39 05  | mov dword ptr ds:[0],539                |
0186799C | E9799C | 8B 75 08                 | mov esi,dword ptr ss:[ebp+8]            |
*/

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
	if (!memoryManager.FindPattern(MEMORY_PATTERN_A, me32.modBaseAddr, me32.modBaseSize, offset)) {
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
	if (!memoryManager.FindPattern(MEMORY_PATTERN_B, me32.modBaseAddr, me32.modBaseSize, offset)) {
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

