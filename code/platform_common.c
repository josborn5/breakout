#include "../../win32-platform/bin/platform.hpp"

static bool IsPressed(const Input &input, int button)
{
	bool isPressed = input.buttons[button].isDown && input.buttons[button].wasDown;
	return isPressed;
}

static bool IsReleased(const Input &input, int button)
{
	bool isReleased = !input.buttons[button].isDown && input.buttons[button].wasDown;
	return isReleased;
}

static bool IsDown(const Input &input, int button)
{
	bool isReleased = input.buttons[button].isDown;
	return isReleased;
}