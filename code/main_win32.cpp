#include <windows.h>
#include "../../win32-platform/code/gentle_giant_win32.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	gentle::Win32Main(instance);
}