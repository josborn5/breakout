
Vector2D ballPosition;
Vector2D ballVelocity;
Vector2D player;

b32 initialized = false;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	int verticalStartPos = 20 - Y_DIM_BASE;
	
	if (!initialized)
	{
		initialized = true;
		ballVelocity.y = -10;
		ballVelocity.x = 5;
		ballPosition.y = verticalStartPos;
		ballPosition.x = 0;
	}
	
	const float speed = 80;	// speed measured in game units / second
	const int batThickness = 2;

	ClearScreen(&renderBuffer, 0x551100);

	// ball
	ballPosition.x += (ballVelocity.x * dt);
	ballPosition.y += (ballVelocity.y * dt);
	float newBallX = ballPosition.x + (ballVelocity.x * dt);
	float newBallY = ballPosition.y + (ballVelocity.y * dt);
	if (newBallY > Y_DIM_BASE || newBallY < -Y_DIM_BASE)
	{
		ballVelocity.y = -ballVelocity.y;
		newBallY = ballPosition.y;
	}
	if (newBallX > X_DIM_BASE || newBallX < -X_DIM_BASE)
	{
		ballVelocity.x = -ballVelocity.x;
		newBallX = ballPosition.x;
	}
	ballPosition.x = newBallX;
	ballPosition.y = newBallY;

	DrawRect(&renderBuffer, 0x0000FF, (Vector2D){ batThickness, batThickness }, ballPosition);

	// bat
	DrawRect(&renderBuffer, 0x00FF00, (Vector2D){ 10, batThickness }, (Vector2D) { input->mouse.x, verticalStartPos });
	
}