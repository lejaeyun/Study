
#include "pch.h"
#include "Source.h"

DWORD FindProcessID(LPCTSTR szProcessName)
{
	DWORD dwPID = 0xFFFFFFFF;
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;

	// Get the snapshot of the system
	pe.dwSize = sizeof(PROCESSENTRY32); // 296d (128h)
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL); // ProcessID = 0

	// find process
	Process32First(hSnapShot, &pe);
	do
	{
		if (!_stricmp(szProcessName, pe.szExeFile))
		{
			dwPID = pe.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	return dwPID;
}
