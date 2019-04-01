#pragma once

#include <stdio.h>
#include <Windows.h>
#include <detours.h>

#pragma comment(lib,"detours.lib")

constexpr auto TARGET_PROC = (L"Nox.exe");

void Patch();
void UnPatch();
void Attach();
void Detach();
DWORD WINAPI ThreadProc(LPVOID lParam);