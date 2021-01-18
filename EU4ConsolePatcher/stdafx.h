#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

#ifdef _DEBUG
#define DEBUG(x) std::cerr << x << std::endl;
#else
#define DEBUG(x)
#endif
