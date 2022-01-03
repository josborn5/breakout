#include "platform.h"

static bool IsPressed(Input *input, int button)
{
	bool isPressed = input->buttons[button].isDown && input->buttons[button].wasDown;
	return isPressed;
}

static bool IsReleased(Input *input, int button)
{
	bool isReleased = !input->buttons[button].isDown && input->buttons[button].wasDown;
	return isReleased;
}

static bool IsDown(Input *input, int button)
{
	bool isReleased = input->buttons[button].isDown;
	return isReleased;
}