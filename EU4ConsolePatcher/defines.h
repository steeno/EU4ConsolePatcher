#pragma once
#define OPCODE_JMP 0xEB

#define MEMORY_PATTERN_A "74 ?? C6 44 24 28 00 41 B8 35 00 00 00"
#define MEMORY_PATTERN_B "74 ?? C7 04 25 00 00 00 00 39 05 00 00"

#define TARGET_PROCESS_NAME "eu4.exe"
#define TARGET_MODULE_NAME "eu4.exe"

#define MAX_MODULE_SNAPSHOT_RETRY_COUNT 10