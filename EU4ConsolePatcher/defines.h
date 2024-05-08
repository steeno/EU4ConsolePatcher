#pragma once
constexpr auto OPCODE_JMP = 0xEB;

// 00007FF6EF522A77 | 74 5F                         | je eu4.7FF6EF522AD8                     |
// RVA: 014B2A77
constexpr auto MEMORY_PATTERN_A = "74 ?? C6 44 24 40 00 B9";
constexpr auto PATCH_A_FALLBACK = 0x171DF5D;
// 00007FF6EF522C67 | 74 0B                         | je eu4.7FF6EF522C74                     |
// RVA: 014B2C67
constexpr auto MEMORY_PATTERN_B = "74 ?? C7 04";
constexpr auto PATCH_B_FALLBACK = 0x171E27B;

constexpr auto TARGET_PROCESS_NAME = "eu4.exe";
constexpr auto TARGET_MODULE_NAME = "eu4.exe";

constexpr auto MAX_MODULE_SNAPSHOT_RETRY_COUNT = 10;