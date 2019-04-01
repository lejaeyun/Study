
#include "pch.h"
#include "Source.h"

BOOL EjectDll(DWORD dwPID, LPCTSTR DllName)
{
	BOOL mf = FALSE, bf = FALSE;
	HANDLE h_Snapshot, h_Process, h_Thread;
	HMODULE h_Module = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE ThreadProc;

	h_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	mf = Module32First(h_Snapshot, &me);
	for (; mf; mf = Module32Next(h_Snapshot, &me))
	{
		if (!_stricmp((LPCTSTR)me.szModule, DllName))
		{
			bf = TRUE;
			break;
		}
	}

	if (!bf)
	{
		CloseHandle(h_Snapshot);
		return FALSE;
	}

	h_Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	h_Module = GetModuleHandle("kernel32.dll");
	ThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(h_Module, "FreeLibrary");
	h_Thread = CreateRemoteThread(h_Process, NULL, 0, ThreadProc, me.modBaseAddr, 0, NULL);
	WaitForSingleObject(h_Thread, INFINITE);

	CloseHandle(h_Thread);
	CloseHandle(h_Process);
	CloseHandle(h_Snapshot);

	return TRUE;
}