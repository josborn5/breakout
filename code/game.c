const float INIT_BALL_SPEED = -50.0f;
const float MAX_BALL_SPEED = 100.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;

const float BLOCK_WIDTH = 6.0f;
const float BLOCK_HEIGHT = 3.0f;

const float BALL_SIZE = 2.0f;

const float BAT_WIDTH = 10.0f;
const float BAT_HEIGHT = 3.0f;

Vector2D ballPosition;
Vector2D ballVelocity;
Vector2D ballHalfSize;

Vector2D playerPosition;
Vector2D playerVelocity;
Vector2D playerHalfSize;

struct {
	Vector2D position;
	Vector2D halfSize;
	uint32_t color;
	b32 exists;
} typedef Block;

Block blocks[64];
int nextBlock;

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

		ballHalfSize = (Vector2D){ BALL_SIZE, BALL_SIZE };

		playerPosition.y = 20 - Y_DIM_BASE;
		playerVelocity.y = 0;
		playerVelocity.x = 0;
		playerHalfSize = (Vector2D){ BAT_WIDTH, BAT_HEIGHT };

		const int BLOCK_ROW_COUNT = 2;
		const int BLOCK_COL_COUNT = 4;
		for (int y = 0; y < BLOCK_ROW_COUNT; y += 1)
		{
			for (int x = 0; x < BLOCK_COL_COUNT; x += 1)
			{
				Block *block = blocks + nextBlock;
				nextBlock++;
				if (nextBlock >= ArrayCount(blocks))
				{
					nextBlock = 0;
				}
				block->exists = 1;
				block->halfSize.x = BLOCK_WIDTH;
				block->halfSize.y = BLOCK_HEIGHT;
				block->position.x = 2 * x * BLOCK_WIDTH;
				block->position.y = 2 * y * BLOCK_HEIGHT;
				block->color = MakeColorFromGrey((x + y) * 20);
			}
		}
	}

	float oldPlayerPositionX = playerPosition.x;
	playerPosition.x = TransformPixelCoordToGameCoord(&renderBuffer, input->mouse.x, input->mouse.y).x;
	playerVelocity.x = (playerPosition.x - oldPlayerPositionX) / dt;

	char debugStringBuffer[256];
	sprintf_s(debugStringBuffer, 256, "dt %f \n", dt);
	OutputDebugStringA(debugStringBuffer);

	sprintf_s(debugStringBuffer, 256, "pos.x %f \n", playerPosition.x);
	OutputDebugStringA(debugStringBuffer);

	sprintf_s(debugStringBuffer, 256, "vel.x %f \n", playerVelocity.x);
	OutputDebugStringA(debugStringBuffer);

	// ball
	ballPosition = AddVector2D(ballPosition, MultiplyVector2D(ballVelocity, dt));

	// Check for collision between ball and bat
	if (ballVelocity.y < 0 && AABBCollideRectToRect(ballHalfSize, ballPosition, playerHalfSize, playerPosition))
	{
		ballVelocity.y *= -1.0f;
		ballVelocity.x += (0.25f * playerVelocity.x);	// Need to tune the multiplier here so mouse speed can control ball speed without just clipping to MAX_BALL_SPEED
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

	// blocks
	for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
	{
		if (!block->exists) continue;

		DrawRect(&renderBuffer, block->color, block->halfSize, block->position);
	}

	// bat
	DrawRect(&renderBuffer, BAT_COLOR, playerHalfSize, playerPosition);
	
}