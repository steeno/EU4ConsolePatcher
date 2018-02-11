#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

#ifdef _DEBUG
#define DEBUG(x) std::wcerr << x << std::endl;
#else
#define DEBUG(x)
#endif
