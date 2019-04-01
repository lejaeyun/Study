#include "Source.h"

#define MSU TRUE
#define KEYDOWN 1
#define KEYUP 2
#define KEYPRESS 3

void Nox_PostMessage(HWND handler, DWORD x, DWORD y, int kbd_type) {
	if (kbd_type == FALSE)
	{
		LONG Loc;
		Loc = MAKELPARAM((LONG)(x / 1.5), (LONG)(y / 1.5));
		printf("%x", Loc);
		PostMessage(handler, WM_LBUTTONDOWN, 1, Loc);
		Sleep(30);
		PostMessage(handler, WM_LBUTTONUP, 0, Loc);
		Sleep(30);
	}
	else if (kbd_type == KEYDOWN)
	{
		PostMessage(handler, WM_KEYDOWN, x, y);
		Sleep(30);
	}
	else if (kbd_type == KEYUP)
	{
		PostMessage(handler, WM_KEYUP, x, y | 0xC0000000);
		Sleep(30);
	}
	else if (kbd_type == KEYPRESS)
	{
		PostMessage(handler, WM_KEYDOWN, x, y);
		Sleep(30);
		PostMessage(handler, WM_KEYUP, x, y | 0xC0000000);
		Sleep(30);
	}
}
//SKIP = 1220, 690

int main(int argc, char* argv[])
{

	auto Kbd_Target = FindWindow("Qt5QWindowIcon", "NoxPlayer");
	auto Kbd_Target = FindWindow("Qt5QWindowIcon", "Nox_1");
	auto Msu_Target = FindWindowEx(Kbd_Target, NULL, "Qt5QWindowIcon", "ScreenBoardClassWindow");
	Nox_PostMessage(Kbd_Target, 0x51, MAKELPARAM(1, MapVirtualKeyA(0x51, MAPVK_VK_TO_VSC)), KEYDOWN);
	Nox_PostMessage(Kbd_Target, 0x32, MAKELPARAM(1, MapVirtualKeyA(0x32, MAPVK_VK_TO_VSC)), KEYPRESS);
	Nox_PostMessage(Kbd_Target, 0x51, MAKELPARAM(1, MapVirtualKeyA(0x51, MAPVK_VK_TO_VSC)), KEYUP);

	return 0;
}
	