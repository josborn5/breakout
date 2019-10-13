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

	BUTTON_COUNT
};

struct
{
	Vector2D mouse;
	Button buttons[BUTTON_COUNT];
} typedef Input;

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