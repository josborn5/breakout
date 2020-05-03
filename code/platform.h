#ifndef PLATFORM_H
#define PLATFORM_H

#include "math.h"
#include "utils.h"

struct
{
	int width;
	int height;
	uint32_t *pixels;
} typedef RenderBuffer;

struct
{
	b32 isDown;
	b32 wasDown;
	b32 keyUp;
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
	Vector2D mouse;
	Button buttons[BUTTON_COUNT];
} typedef Input;

#endif