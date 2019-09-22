
Vector2D player;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	const float speed = 20;	// speed measured in game units / second
	float positionDelta = speed * dt;

	if (IsPressed(input, BUTTON_LEFT)) player.x -= positionDelta;
	if (IsPressed(input, BUTTON_RIGHT)) player.x += positionDelta;
	if (IsPressed(input, BUTTON_UP)) player.y += positionDelta;
	if (IsPressed(input, BUTTON_DOWN)) player.y -= positionDelta;

	ClearScreen(&renderBuffer, 0x551100);
	DrawRect(&renderBuffer, 0xFFFF00, (Vector2D){ 10, 10 }, player);
}