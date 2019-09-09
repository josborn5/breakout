int playerX = 20;
int playerY = 20;

const int playerWidth = 100;
const playerHeight = 20;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer)
{
	ClearScreen(&renderBuffer, 0x551100);

	if (IsPressed(input, BUTTON_LEFT)) playerX -= 2;
	if (IsPressed(input, BUTTON_RIGHT)) playerX += 2;
	if (IsPressed(input, BUTTON_UP)) playerY += 2;
	if (IsPressed(input, BUTTON_DOWN)) playerY -= 2;

	DrawRectInPixels(&renderBuffer, 0xFFFF00, playerX, playerY, playerX + playerWidth, playerY + playerHeight);
}