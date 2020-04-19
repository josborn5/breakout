#include <stdint.h>
#include <windows.h>

struct
{
	// platform independent
	int width;
	int height;
	uint32_t *pixels;

	// platform dependent
	BITMAPINFO bitmapInfo;
} typedef RenderBuffer;

#include "utils.h"	// this needs to be first as it defines some #defines used in the other *.c files
#include "math.c"
#include "collision.c"
#include "world_transforms.c"
#include "software_rendering.c"
#include "platform_common.c"
#include "game.c"

global_variable b32 isRunning = true;
global_variable b32 showRectangle = false;
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

internal void ProcessButton(int vkCode, b32 isDown, b32 wasDown, Button *button, int vkButton)
{
	if (vkCode == vkButton)
	{
		button->isDown = isDown;
		button->wasDown = wasDown;
		button->keyUp = (!isDown && wasDown);
	}
}

internal void ResetButtons(Input *input)
{
	for (int i = 0; i < BUTTON_COUNT; i += 1)
	{
		if (input->buttons[i].keyUp)
		{
			input->buttons[i].wasDown = false;
			input->buttons[i].keyUp = false;
		}
	}
}

int WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSA windowClass = {0};
	windowClass.style = CS_HREDRAW|CS_VREDRAW;
	windowClass.lpszClassName = "Game window class";
	windowClass.lpfnWndProc = windowCallback;

	RegisterClassA(&windowClass);

	HWND window = CreateWindowExA(0, windowClass.lpszClassName, "Breakout Clone",
									WS_VISIBLE|WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT, CW_USEDEFAULT,
									1280, 720, 0, 0, 0, 0);
	HDC hdc = GetDC(window);
	Input input = {0};
	LARGE_INTEGER lastCounter;
	QueryPerformanceCounter(&lastCounter);

	LARGE_INTEGER performanceFrequencyLargeInt;
	QueryPerformanceFrequency(&performanceFrequencyLargeInt);
	float frequencyCounter = (float)performanceFrequencyLargeInt.QuadPart;
	float lastDt = 0.01666f;

	while(isRunning)
	{
		// input
		MSG message;
		while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
		{
			switch(message.message)
			{
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					uint32_t vkCode = (uint32_t)message.wParam;
					b32 wasDown = ((message.lParam & (1 << 30)) != 0);	// message.lParam is 1 for KEYUP and 1 or 0 for KEYDOWN. i.e. 1 means was pressed, 0 means was not pressed
					b32 isDown = ((message.lParam & (1 << 31)) == 0);	// message.lParam is always 0 for KEYDOWN & 1 for KEYUP. i.e. 0 means is pressed, 1 means is not pressed

					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_LEFT], VK_LEFT);
					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_RIGHT], VK_RIGHT);
					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_UP], VK_UP);
					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_DOWN], VK_DOWN);
					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_PAUSE], VK_SPACE);
					ProcessButton(vkCode, isDown, wasDown, &input.buttons[BUTTON_RESET], 0x52); // 'R' key

					break;
				}

				default:
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}

		// Get mouse position
		POINT mousePointer;
		GetCursorPos(&mousePointer);	// mousePointer in screen coord
		ScreenToClient(window, &mousePointer);	// convert screen coord to window coord
		input.mouse.x = mousePointer.x;
		input.mouse.y = renderBuffer.height - mousePointer.y;

		// simulation
		SimulateGame(&input, renderBuffer, lastDt);

		ResetButtons(&input);

		// render
		StretchDIBits(hdc,
			0, 0, renderBuffer.width, renderBuffer.height,
			0, 0, renderBuffer.width, renderBuffer.height,
			renderBuffer.pixels, &renderBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		// Get the frame time
		LARGE_INTEGER currentCounter;
		QueryPerformanceCounter(&currentCounter);
		float counterDiff = (float)(currentCounter.QuadPart - lastCounter.QuadPart);

		lastDt = counterDiff / frequencyCounter;

		// reset for next frame
		lastCounter = currentCounter;
	}

}