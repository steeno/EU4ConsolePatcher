#pragma once
struct patchInfo_t {
	DWORD baseAddress;
	DWORD offset;
	BYTE* opcodes;
	SIZE_T length;
};