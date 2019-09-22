struct
{
	b32 isDown;
	b32 changed;
} typedef Button;

enum
{
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,

	BUTTON_COUNT
};

struct
{
	Vector2D mouse;
	Button buttons[BUTTON_COUNT];
} typedef Input;

internal b32 IsPressed(Input *input, int button)
{
	b32 isPressed = input->buttons[button].isDown && input->buttons[button].changed;
	return isPressed;
}

internal b32 IsReleased(Input *input, int button)
{
	b32 isReleased = !input->buttons[button].isDown && input->buttons[button].changed;
	return isReleased;
}

internal b32 IsDown(Input *input, int button)
{
	b32 isReleased = input->buttons[button].isDown;
	return isReleased;
}