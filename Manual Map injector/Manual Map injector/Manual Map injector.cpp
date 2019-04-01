// Manual Map injector.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

#include "My Dll Byte.h"
#include "Manual Map Header.h"


// Manual Map Injection From Memory || Mo1ra - Ensar

DWORD kjclkjclk2jlkjsafd()
{
	SetConsoleTitle("Mo1ra - Manual Map Injector");
	system("color 4");
	cout << "Developed by Mo1ra\n";
	cout << "Oyun bekleniyor..\n";
	while (true)
	{
		if (FindProcess("zula.exe"))
		{
			PVOID rData = reinterpret_cast<char*>(rawData);
			//
			pIDH = (PIMAGE_DOS_HEADER)/*Memory*/rData;
			pINH = (PIMAGE_NT_HEADERS)((LPBYTE)/*Memory*/rData + pIDH->e_lfanew);

			DWORD pid = MyGetProcessId(/*Process Name*/"zula.exe");
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

			image = VirtualAllocEx(hProcess, NULL, pINH->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			WriteProcessMemory(hProcess, image, /*Memory*/rData, pINH->OptionalHeader.SizeOfHeaders, NULL);
			pISH = (PIMAGE_SECTION_HEADER)(pINH + 1);
			for (i = 0; i < pINH->FileHeader.NumberOfSections; i++)
			{
				WriteProcessMemory(hProcess, (PVOID)((LPBYTE)image + pISH[i].VirtualAddress),
					(PVOID)((LPBYTE)rData/*Memory*/ + pISH[i].PointerToRawData), pISH[i].SizeOfRawData, NULL);
			}
			mem = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			memset(&ManualInject, 0, sizeof(MANUAL_INJECT));

			ManualInject.ImageBase = image;
			ManualInject.NtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)image + pIDH->e_lfanew);
			ManualInject.BaseRelocation = (PIMAGE_BASE_RELOCATION)((LPBYTE)image + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			ManualInject.ImportDirectory = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)image + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
			ManualInject.fnLoadLibraryA = LoadLibraryA;
			ManualInject.fnGetProcAddress = GetProcAddress;

			WriteProcessMemory(hProcess, mem, &ManualInject, sizeof(MANUAL_INJECT), NULL);
			WriteProcessMemory(hProcess, (PVOID)((PMANUAL_INJECT)mem + 1), LoadDll, (DWORD)LoadDllEnd - (DWORD)LoadDll, NULL);
			system("color 2");
			system("cls");
			cout << "Injecting..";
			Sleep(6000);

			hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((PMANUAL_INJECT)mem + 1), mem, 0, NULL);
			cout << "Injected!";
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &ExitCode);
			ExitProcess(0);
		}
	}
}

int main()
{
	kjclkjclk2jlkjsafd();
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
