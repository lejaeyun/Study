#pragma once

#include "stdio.h"
#include "windows.h"
#include "tlhelp32.h"

DWORD FindProcessID(LPCTSTR szProcessName);
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName);
BOOL EjectDll(DWORD dwPID, LPCTSTR DllName);