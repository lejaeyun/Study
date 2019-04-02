
#include "pch.h"
#include "Source.h"

BOOL InjectDll(DWORD dwPID, LPCTSTR DllName)
{
	HANDLE hProcess, hThread;
	LPVOID pRemoteBuf;
	DWORD dwBufSize = lstrlen(DllName) + 1;
	LPTHREAD_START_ROUTINE pThreadProc;

	//HMODULE h_Ntdll = LoadLibrary("NTDLL.DLL");
	//pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("ntdll.dll"), "LoadLibraryA");

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwPID);
	if (hProcess == NULL)
	{
		printf("OpenPrecess Failed! %x\n", hProcess);
		return FALSE;
	}


	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	int A = WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)DllName, dwBufSize, NULL);

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

