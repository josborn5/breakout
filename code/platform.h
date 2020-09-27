#ifndef PLATFORM_H
#define PLATFORM_H

#include "utils.h"

struct
{
	uint32_t* pixels;
	int width;
	int height;
	int pitch;
	int bytesPerPixel; // = 4;
	int xMax;
	int yMax;
} typedef RenderBuffer;

struct
{
	b32 isDown;
	b32 wasDown;
	b32 keyUp;
	int halfTransitionCount;
	int endedDown;
} typedef Button;

enum
{
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_PAUSE,
	BUTTON_RESET,

	BUTTON_COUNT
};

struct
{
	int x;
	int y;
} typedef Position;

struct
{
	Position mouse;
	Button buttons[BUTTON_COUNT];
} typedef Input;

struct
{
	b32 IsInitialized;
	uint64_t PermanentStorageSpace;
	uint64_t TransientStorageSpace;
	void* PermanentStorage;
	void* TransientStorage;
} typedef GameMemory;

#endif