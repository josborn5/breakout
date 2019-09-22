
Vector2D player;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	const float speed = 80;	// speed measured in game units / second
	float positionDelta = speed * dt;

	if (IsPressed(input, BUTTON_LEFT)) player.x -= positionDelta;
	if (IsPressed(input, BUTTON_RIGHT)) player.x += positionDelta;
	if (IsPressed(input, BUTTON_UP)) player.y += positionDelta;
	if (IsPressed(input, BUTTON_DOWN)) player.y -= positionDelta;

	ClearScreen(&renderBuffer, 0x551100);

	const int batThickness = 2;
	DrawRect(&renderBuffer, 0x00FF00, (Vector2D){ 10, batThickness }, (Vector2D) { input->mouse.x, 20 - Y_DIM_BASE });
	
}