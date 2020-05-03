#include "platform.h"

internal b32 IsPressed(Input *input, int button)
{
	b32 isPressed = input->buttons[button].isDown && input->buttons[button].wasDown;
	return isPressed;
}

internal b32 IsReleased(Input *input, int button)
{
	b32 isReleased = !input->buttons[button].isDown && input->buttons[button].wasDown;
	return isReleased;
}

internal b32 IsDown(Input *input, int button)
{
	b32 isReleased = input->buttons[button].isDown;
	return isReleased;
}