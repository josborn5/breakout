
Vector2D player;

const int playerWidth = 100;
const playerHeight = 20;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer)
{
	ClearScreen(&renderBuffer, 0x551100);

	if (IsPressed(input, BUTTON_LEFT)) player.x -= 2;
	if (IsPressed(input, BUTTON_RIGHT)) player.x += 2;
	if (IsPressed(input, BUTTON_UP)) player.y += 2;
	if (IsPressed(input, BUTTON_DOWN)) player.y -= 2;

	DrawRect(&renderBuffer, 0xFFFF00, (Vector2D){ 10, 10 }, player);
}