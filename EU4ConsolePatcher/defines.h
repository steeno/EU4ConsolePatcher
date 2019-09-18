#pragma once
#define OPCODE_JMP 0xEB

#define MEMORY_PATTERN_A L"\x74\x00\xC6\x44\x24\x28\x00\x00\x00\x00\x00\x00\x00\x48"
#define MEMORY_SIGNATURE_A L"x?xxxxx??????x"
#define MEMORY_PATTERN_B L"\x74\x00\xC7\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0F"
#define MEMORY_SIGNATURE_B L"x?xx?????????x"

#define TARGET_PROCESS_NAME L"eu4.exe"
#define TARGET_MODULE_NAME L"eu4.exe"