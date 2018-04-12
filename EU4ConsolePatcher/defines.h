#pragma once
#define OPCODE_JMP 0xEB

#define MEMORY_PATTERN_A L"\x74\x18\x6A\x35\x68????\x8D\x4D\xD8"
#define MEMORY_PATTERN_B L"\x74\x0A\xC7\x05????????\x8B\x75\x08\x8A\x85\x7C\xFF\xFF\xFF"

#define TARGET_PROCESS_NAME L"eu4.exe"
#define TARGET_MODULE_NAME L"eu4.exe"