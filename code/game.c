const float INIT_BALL_SPEED = -40.0f;
const float MAX_BALL_SPEED = 100.0f;
const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;

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
		ballVelocity.y = INIT_BALL_SPEED;
		ballVelocity.x = 5;

		ballPosition.y = 0;
		ballPosition.x = 0;

		ballHalfSize = (Vector2D){ 2, 2 };

		playerPosition.y = 20 - Y_DIM_BASE;
		playerVelocity.y = 0;
		playerVelocity.x = 0;
		playerHalfSize = (Vector2D){ 10, 2 };
	}

	float oldPlayerPositionX = playerPosition.x;
	playerPosition.x = TransformPixelCoordToGameCoord(&renderBuffer, input->mouse.x, input->mouse.y).x;
	playerVelocity.x = (playerPosition.x - oldPlayerPositionX) / dt;

	// ball
	ballPosition = AddVector2D(ballPosition, MultiplyVector2D(ballVelocity, dt));

	// Check for collision between ball and bat
	if (AABBCollideRectToRect(ballHalfSize, ballPosition, playerHalfSize, playerPosition))
	{
		ballVelocity.y *= -1.0f;
		ballVelocity.x += (0.75f * playerVelocity.x);	// 0.75 is a smudge factor for inefficient transfer of momentum
		// if mouse is moving crazy fast, ball will start moving crazy fast, so clamp to a maximum ball speed
		ballVelocity.x = ClampFloat(-MAX_BALL_SPEED, ballVelocity.x, MAX_BALL_SPEED);
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

	// ball
	ClearScreenAndDrawRect(&renderBuffer, BALL_COLOR, BACKGROUND_COLOR, ballHalfSize, ballPosition);

	// bat
	DrawRect(&renderBuffer, BAT_COLOR, playerHalfSize, playerPosition);
	
}