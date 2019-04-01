
#include "stdafx.h"
#include "Source.h"

typedef int (WINAPI* LPFN_MBA)(HWND, LPCWSTR, LPCWSTR, UINT);
LPFN_MBA oMessageBoxW = NULL;

int WINAPI hMessageBoxA(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	return oMessageBoxW(hWnd, L"HOOKED", lpCaption, uType);
}

void Attach()
{
	DetourAttach((PVOID*)&oMessageBoxW, hMessageBoxA);
}

void Detach()
{
	DetourDetach((PVOID*)&oMessageBoxW, hMessageBoxA);
}

void Patch()
{
	HMODULE user32 = GetModuleHandle(TEXT("user32.dll"));
	if (user32 != NULL)
	{
		oMessageBoxW = (LPFN_MBA)GetProcAddress(user32, "MessageBoxW");
		if (oMessageBoxW != NULL)
		{
			Attach();
		}
	}
}

void UnPatch()
{
	if (oMessageBoxW != NULL)
	{
		Detach();
	}
}


DWORD WINAPI ThreadProc(LPVOID lParam)
{

	MessageBox(NULL, L"OK", L"메시지 박스", MB_OK);
	return 0;

}

void Go_Thread(HANDLE *h_Thread)
{
	MessageBox(NULL, L"OK2", L"메시지 박스", MB_OK);
	*h_Thread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	MessageBox(NULL, L"OK3", L"메시지 박스", MB_OK);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	HANDLE h_Thread = NULL;
	MSG msg;
	if (fdwReason == DLL_PROCESS_ATTACH)
	{

		DisableThreadLibraryCalls(hinstDLL);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		Patch();
		LONG Error = DetourTransactionCommit();
		if (Error != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed To Attach", L"ERROR", MB_OK);
		}

		MessageBox(NULL, L"OK", L"메시지 박스", MB_OK);


		//Go_Thread(&h_Thread);

	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		DisableThreadLibraryCalls(hinstDLL);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		UnPatch();
		LONG Error = DetourTransactionCommit();
		if (Error != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to Detach", L"ERROR", MB_OK);
		}

		CloseHandle(h_Thread);
	}

	return TRUE;
}

