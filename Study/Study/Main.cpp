
#include "pch.h"
#include "Source.h"

#define DEF_PROC_NAME ("gamer.exe")
#define DEF_DLL_PATH ("C:\TEST\\test_32.dll")
#define DEF_DLL_NAME ("test_32.dll")

int main(int argc, char* argv[])
{	
	//CreateThread(NULL, 0, exampleThread, 0, 0, NULL);
	// find process
	BOOL result;
	DWORD dwPID = 0xFFFFFFFF;

	dwPID = FindProcessID(DEF_PROC_NAME);
	if (dwPID == 0xFFFFFFFF)
	{
		printf("There is no <%s> process!\n", DEF_PROC_NAME);
		return 1;
	}
	printf("PID : %d \n", dwPID);
	
	result = InjectDll(dwPID, DEF_DLL_PATH);
	if (result == TRUE)
		printf("Dll Injection Success\n");
	else if (result == FALSE)
		printf("Dll Injection FAILED\n");

	printf("\n\n\n\n");
	Sleep(1500);

	result = EjectDll(dwPID, DEF_DLL_NAME);
	if (result == TRUE)
		printf("Dll Ejection Success\n");
	else if (result == FALSE)
		printf("Dll Ejection FAILED\n");

	system("pause");
	return 0;
	
}
