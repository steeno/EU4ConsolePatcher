#include "stdafx.h"
#include "ProcessManager.h"
#include "ModuleManager.h"
#include "MemoryManager.h"

/* v1.23.0
>eu4.exe
00E7AAB1: 74->EB
00E7AC80: 74->EB
*/
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
#define OFFSET_A 0xE977C1
#define OFFSET_B 0xE97990
#define OPCODE_JE 0x74
#define OPCODE_JMP 0xEB

#define STATUS_ERROR -1;
#define STATUS_SUCCESS 0;

#define TARGET_PROCESS_NAME L"eu4.exe"

int main() {
	ProcessManager processManager;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!processManager.FindProcess(TARGET_PROCESS_NAME, pe32)) {
		std::wcerr << L"Process: " << TARGET_PROCESS_NAME << L" could not be found." << std::endl;
		getchar();
		return STATUS_ERROR;
	}
	std::wcout << L"PID: " << pe32.th32ProcessID << std::endl << \
		L"PNAME: " << pe32.szExeFile << std::endl;

	ModuleManager moduleManager(pe32.th32ProcessID);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!moduleManager.FindModule(pe32.szExeFile, me32)) {
		std::wcerr << L"Module: " << pe32.szExeFile << L" could not be found." << std::endl;
		getchar();
		return STATUS_ERROR;
	}
	std::wcout << L"MNAME: " << me32.szModule << std::endl << \
		L"MBASE: " << std::hex << me32.modBaseAddr << std::endl << \
		L"MSIZE: " << me32.modBaseSize << std::endl;

	MemoryManager memoryManager(pe32.th32ProcessID);
	const DWORD baseAddress = (DWORD)me32.modBaseAddr + OFFSET_A;
	const BYTE buffer[] = {
		OPCODE_JMP
	};
	if (!memoryManager.Patch(baseAddress, buffer, sizeof(buffer))) {
		std::wcerr << L"Address: " << std::hex << baseAddress << L" could not be patched." << std::endl;
		getchar();
		return STATUS_ERROR;
	}
	std::wcout << L"Address: " << std::hex << baseAddress << L" patched successfully." << std::endl;

	const DWORD baseAddress2 = (DWORD)me32.modBaseAddr + OFFSET_B;
	const BYTE buffer2[] = {
		OPCODE_JMP
	};
	if (!memoryManager.Patch(baseAddress2, buffer2, sizeof(buffer2))) {
		std::wcerr << L"Address: " << std::hex << baseAddress2 << L" could not be patched." << std::endl;
		getchar();
		return STATUS_ERROR;
	}
	std::wcout << L"Address: " << std::hex << baseAddress2 << L" patched successfully." << std::endl;

	getchar();
	return STATUS_SUCCESS;
}

