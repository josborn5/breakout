#include <stdint.h>
#include <windows.h>
#include "utils.c"

struct
{
	// platform independent
	int width;
	int height;
	uint32_t *pixels;

	// platform dependent
	BITMAPINFO bitmapInfo;
} typedef RenderBuffer;

#include "software_rendering.c"
#include "math.c"

global_variable RenderBuffer renderBuffer;

internal LRESULT windowCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	
	switch(message)
	{
		case WM_CLOSE:
		case WM_DESTROY:
		{
			isRunning = false;
			break;
		}

		case WM_SIZE:
		{
			// Get the width & height of the window
			RECT rect;
			GetWindowRect(hWnd, &rect);
			renderBuffer.width = rect.right - rect.left;
			renderBuffer.height = rect.bottom - rect.top;

			// Allocate memory for the current window size
			if (renderBuffer.pixels)
			{
				VirtualFree(renderBuffer.pixels, 0, MEM_RELEASE);
			}
			renderBuffer.pixels = VirtualAlloc(0, sizeof(uint32_t) * renderBuffer.width * renderBuffer.height, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

			// Fill the bitmap info
			renderBuffer.bitmapInfo.bmiHeader.biSize = sizeof(renderBuffer.bitmapInfo.bmiHeader);
			renderBuffer.bitmapInfo.bmiHeader.biWidth = renderBuffer.width;
			renderBuffer.bitmapInfo.bmiHeader.biHeight = renderBuffer.height;
			renderBuffer.bitmapInfo.bmiHeader.biPlanes = 1;
			renderBuffer.bitmapInfo.bmiHeader.biBitCount = 32;
			renderBuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;
			break;
		}

		default:
		{
			result = DefWindowProcA(hWnd, message, wParam, lParam);
		}
	}
	return result;
}

int WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSA windowClass = {0};
	windowClass.style = CS_HREDRAW|CS_VREDRAW;
	windowClass.lpszClassName = "Game window class";
	windowClass.lpfnWndProc = windowCallback;

	RegisterClassA(&windowClass);

	HWND window = CreateWindowExA(0, windowClass.lpszClassName, "Super Smashing Great",
									WS_VISIBLE|WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT, CW_USEDEFAULT,
									1280, 720, 0, 0, 0, 0);
	HDC hdc = GetDC(window);

	while(isRunning)
	{
		// input
		MSG message;
		while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// simulation
		ClearScreen(&renderBuffer, 0x551100);
		DrawRectInPixels(&renderBuffer, 0xFFFF00, 200, 200, 500, 500);

		// render
		StretchDIBits(hdc,
			0, 0, renderBuffer.width, renderBuffer.height,
			0, 0, renderBuffer.width, renderBuffer.height,
			renderBuffer.pixels, &renderBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	}

}