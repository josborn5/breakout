
Vector2D ballPosition;
Vector2D ballVelocity;
Vector2D ballHalfSize;
Vector2D playerPosition;
Vector2D playerVelocity;
Vector2D playerHalfSize;

b32 initialized = false;

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	if (!initialized)
	{
		initialized = true;
		ballVelocity.y = -40;
		ballVelocity.x = 5;

		ballPosition.y = 0;
		ballPosition.x = 0;

		ballHalfSize = (Vector2D){ 2, 2 };

		playerPosition.y = 20 - Y_DIM_BASE;
		playerHalfSize = (Vector2D){ 10, 2 };
	}

	playerPosition.x = input->mouse.x;

	ClearScreen(&renderBuffer, 0x551100);

	// ball
	ballPosition = AddVector2D(ballPosition, MultiplyVector2D(ballVelocity, dt));

	// Check for collision between ball and bat
	if (AABBCollideRectToRect(ballHalfSize, ballPosition, playerHalfSize, playerPosition))
	{
		ballVelocity = MultiplyVector2D(ballVelocity, -1.0f);
	}

	// Check for collision between ball and vertical boundaries of world
	if ((ballVelocity.x < 0 && AABBCollideRectToVertical(ballHalfSize, ballPosition, -X_DIM_BASE))
		|| (ballVelocity.x > 0 && AABBCollideRectToVertical(ballHalfSize, ballPosition, X_DIM_BASE)))
	{
		ballVelocity.x = -1.0f * ballVelocity.x;
	}

	// Check for collision between ball and horizontal boundaries of world
	if (ballVelocity.y > 0 && AABBCollideRectToHorizontal(ballHalfSize, ballPosition, Y_DIM_BASE))
	{
		ballVelocity.y = -1.0f * ballVelocity.y;
	}

	DrawRect(&renderBuffer, 0x0000FF, ballHalfSize, ballPosition);

	// bat
	DrawRect(&renderBuffer, 0x00FF00, playerHalfSize, playerPosition);
	
}