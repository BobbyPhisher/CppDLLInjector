#pragma once

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcID(const char* targetProc);
