#pragma once
#define OPCODE_JMP 0xEB

// 00007FF6EF522A77 | 74 5F                         | je eu4.7FF6EF522AD8                     |
// RVA: 014B2A77
#define MEMORY_PATTERN_A "74 ?? C6 44 24 28 00 41 B8 35 00 00 00"
#define PATCH_A_FALLBACK 0x014B2A77
// 00007FF6EF522C67 | 74 0B                         | je eu4.7FF6EF522C74                     |
// RVA: 014B2C67
#define MEMORY_PATTERN_B "74 ?? C7 04 25 00 00 00 00 39 05 00 00"
#define PATCH_B_FALLBACK 0x014B2C67

#define TARGET_PROCESS_NAME "eu4.exe"
#define TARGET_MODULE_NAME "eu4.exe"

#define MAX_MODULE_SNAPSHOT_RETRY_COUNT 10