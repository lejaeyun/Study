#include "pch.h"
#include "Source.h"

#define STATUS_SUCCESS       ((NTSTATUS)0x00000000)
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022)
#define INOUT

typedef struct _CLIENT_ID {
	ULONG UniqueProcess;
	ULONG UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetInformation,
	MemoryMappedFilenameInformation,
	MemoryRegionInformation,
	MemoryWorkingSetExInformation,
	MemorySharedCommitInformation,
	MemoryImageInformation,
	MemoryRegionInformationEx,
	MemoryPrivilegedBasicInformation
} MEMORY_INFORMATION_CLASS;

NTSTATUS NTAPI NtOpenProcess_Hook(OUT PHANDLE ProcessHandle, IN ACCESS_MASK AccessMask,INOUT POBJECT_ATTRIBUTES ObjectAttributes, INOUT PCLIENT_ID ClientId)
{
	if (ClientId->UniqueProcess != NULL && ClientId->UniqueProcess != maple_pid)
	{
		Log(L"Blocking NtOpenProcess PID: %.08X\n", ClientId->UniqueProcess);
		*ProcessHandle = NULL;
		return STATUS_ACCESS_DENIED;
	}

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI NtQuerySystemInformation_Hook(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	switch (SystemInformationClass)
	{
	case SystemProcessInformation:
		memset(SystemInformation, 0, SystemInformationLength);

		if (ReturnLength != NULL)
			*ReturnLength = 0;

		Log(L"Blocking NtQuerySystemInformation SystemInformationClass(%.08X)\n", SystemInformationClass);
		return STATUS_ACCESS_DENIED;

	default:
		break;
	}

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI NtQueryVirtualMemory_Hook(HANDLE ProcessHandle, LPBYTE BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
{
	switch (MemoryInformationClass)
	{
	case MemoryBasicInformation:
		memset(MemoryInformation, 0, MemoryInformationLength);

		if (ReturnLength != NULL)
			*ReturnLength = 0;

		Log(L"[%.08X] Blocking NtQueryVirtualMemory MemoryInformationClass(%.08X) Address: %.08X\n", GetProcessId(ProcessHandle), MemoryInformationClass, BaseAddress);
		return STATUS_SUCCESS;

	case MemoryWorkingSetInformation:
	case MemoryMappedFilenameInformation:
		memset(MemoryInformation, 0, MemoryInformationLength);

		if (ReturnLength != NULL)
			*ReturnLength = 0;

		Log(L"[%.08X] Blocking NtQueryVirtualMemory MemoryInformationClass(%.08X) Address: %.08X\n", GetProcessId(ProcessHandle), MemoryInformationClass, BaseAddress);
		return STATUS_ACCESS_DENIED;

	default:
		break;
	}

	return STATUS_SUCCESS;
}

DWORD_PTR dwWow64Address = 0;
LPVOID lpJmpRealloc = nullptr;

void __declspec(naked) NtOpenProcessHook()
{
	__asm
	{
		push eax
		push[ebp + 0x14] // ClientId
		push[ebp + 0x10] // ObjectAttributes
		push[ebp + 0x0C] // DesiredAccess
		push[ebp + 0x08] // ProcessHandle
		call NtOpenProcess_Hook
		test eax, eax
		jz NtOpenProcess_End
		mov dword ptr[esp], eax
		mov esp, ebp
		pop ebp
		ret 0x0010

		NtOpenProcess_End:
		pop eax
			jmp lpJmpRealloc
	}
}

void __declspec(naked) NtQuerySystemInformationHook()
{
	__asm
	{
		push eax
		push[ebp + 0x14] // ReturnLength
		push[ebp + 0x10] // SystemInformationLength
		push[ebp + 0x0C] // SystemInformation
		push[ebp + 0x08] // SystemInformationClass
		call NtQuerySystemInformation_Hook
		test eax, eax
		jz NtQuerySystemInformation_End
		mov dword ptr[esp], eax
		mov esp, ebp
		pop ebp
		ret 0x0010

		NtQuerySystemInformation_End:
		pop eax
			jmp lpJmpRealloc
	}
}

void __declspec(naked) NtQueryVirtualMemoryHook()
{
	__asm
	{
		push[ebp + 0x1C] // ReturnLength
		push[ebp + 0x18] // MemoryInformationLength
		push[ebp + 0x14] // MemoryInformation
		push[ebp + 0x10] // MemoryInformationClass
		push[ebp + 0x0C] // BaseAddress
		push[ebp + 0x08] // ProcessHandle
		call NtQueryVirtualMemory_Hook
		mov esp, ebp
		pop ebp
		ret 0x0018
	}
}

const DWORD_PTR __declspec(naked) GetWow64Address()
{
	__asm
	{
		mov eax, dword ptr fs : [0xC0]
		ret
	}
}

void __declspec(naked) Wow64Trampoline()
{
	__asm
	{
		cmp eax, 0x26
		jz NtOpenProcessHook
		cmp eax, 0x36
		jz NtQuerySystemInformationHook
		cmp eax, 0x23
		jz NtQueryVirtualMemoryHook
		jmp lpJmpRealloc
	}
}

const LPVOID CreateNewJump(const DWORD_PTR dwWow64Address)
{
	lpJmpRealloc = VirtualAlloc(nullptr, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	(void)memcpy(lpJmpRealloc, (const void *)dwWow64Address, 9);

	return lpJmpRealloc;
}

const void EnableWow64Redirect(const DWORD_PTR dwWow64Address, const LPVOID lpNewJumpLocation)
{
	unsigned char trampolineBytes[] =
	{
		0x68, 0xDD, 0xCC, 0xBB, 0xAA,       // push 0xAABBCCDD
		0xC3,                               // ret
		0xCC, 0xCC, 0xCC                    // padding
	};
	memcpy(&trampolineBytes[1], &lpNewJumpLocation, sizeof(DWORD_PTR));

	DWORD dwOldProtect = 0;
	(void)VirtualProtect((LPVOID)dwWow64Address, 4096, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	(void)memcpy((void *)dwWow64Address, trampolineBytes, sizeof(trampolineBytes));
	(void)VirtualProtect((LPVOID)dwWow64Address, 4096, dwOldProtect, &dwOldProtect);
}

void Wow32Reserved_Hook()
{
	dwWow64Address = GetWow64Address();

	const LPVOID lpNewJumpLocation = CreateNewJump(dwWow64Address);
	EnableWow64Redirect(dwWow64Address, (LPVOID)Wow64Trampoline);
}