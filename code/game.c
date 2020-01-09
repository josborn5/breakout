const float INIT_BALL_SPEED = -75.0f;
const float MAX_BALL_SPEED = 150.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;

const float BLOCK_WIDTH = 12.0f;
const float BLOCK_HEIGHT = 6.0f;

const float BALL_SIZE = 4.0f;

const float BAT_WIDTH = 20.0f;
const float BAT_HEIGHT = 6.0f;

Vector2D prevBallPosition;
Vector2D ballPosition;
Vector2D ballVelocity;
Vector2D ballHalfSize;

Vector2D prevPlayerPosition;
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
b32 isPaused = false;

char debugStringBuffer[256];

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	if (!initialized)
	{
		initialized = true;
		ballVelocity.y = INIT_BALL_SPEED;
		ballVelocity.x = 20;

		ballPosition.y = -10;
		ballPosition.x = 0;

		ballHalfSize = (Vector2D){ BALL_SIZE, BALL_SIZE };

		playerPosition.y = 20 - Y_DIM_BASE;
		prevPlayerPosition.x = TransformPixelCoordToGameCoord(&renderBuffer, input->mouse.x, input->mouse.y).x;
		prevPlayerPosition.y = playerPosition.y;
		playerVelocity.y = 0;
		playerVelocity.x = 0;
		playerHalfSize = (Vector2D){ BAT_WIDTH, BAT_HEIGHT };

		const int BLOCK_ROW_COUNT = 2;
		const int BLOCK_COL_COUNT = 8;
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
				block->position.x = (2 * x * BLOCK_WIDTH) - ((BLOCK_COL_COUNT / 2) * BLOCK_WIDTH);
				block->position.y = 2 * y * BLOCK_HEIGHT;
				block->color = MakeColorFromGrey((x + y) * 20);
			}
		}
		return;
	}

	if (IsReleased(input, BUTTON_PAUSE))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		// cache the previous position
		prevBallPosition.x = ballPosition.x;
		prevBallPosition.y = ballPosition.y;

		// ball
		playerPosition.x = TransformPixelCoordToGameCoord(&renderBuffer, input->mouse.x, input->mouse.y).x;
		playerVelocity.x = (playerPosition.x - prevPlayerPosition.x) / dt;
		ballPosition = AddVector2D(prevBallPosition, MultiplyVector2D(ballVelocity, dt));

		// Check for collision between ball and bat
		if (ballVelocity.y < 0 && AABBCollideRectToRect(ballHalfSize, ballPosition, playerHalfSize, playerPosition))
		{
			ballVelocity.y *= -1.0f;
			// ballVelocity.x += (0.1f * playerVelocity.x);	// Need to tune the multiplier here so mouse speed can control ball speed without just clipping to MAX_BALL_SPEED
			// if mouse is moving crazy fast, ball will start moving crazy fast, so clamp to a maximum ball speed
			// ballVelocity.x = ClampFloat(-MAX_BALL_SPEED, ballVelocity.x, MAX_BALL_SPEED);
		}

		// Define a rect over the path the ball takes in the current timestep. Use this to work our which blocks could be collided with
		Vector2D ballPathBottomLeft = (Vector2D) {MinFloat(ballPosition.x, prevBallPosition.x), MinFloat(ballPosition.y, prevBallPosition.y)};
		Vector2D ballPathTopRight = (Vector2D) {MaxFloat(ballPosition.x, prevBallPosition.x), MaxFloat(ballPosition.y, prevBallPosition.y)};

		float minCollisionTime = dt;

		int ballCollisionResult = None;
		// Check for collision between any boudary of the world
		CheckBlockAndTopsideOfWallCollision(-Y_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult);
		CheckBlockAndUndersideOfWallCollision(Y_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult);
		CheckBlockAndLeftWallCollision(-X_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult);
		CheckBlockAndRightWallCollision(X_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult);

		int blockHitIndex = -1;

		// check for collision between ball and blocks
		for (int i = 0; i < ArrayCount(blocks); i++)
		// for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
		{
			Block *block = &blocks[i];
			if (!block->exists) continue;

			// Check if the block lies on the path the ball takes on the current timestep
			// if(!AABBCollideCornerToRect(block->halfSize, block->position, ballPathTopRight, ballPathBottomLeft)) continue;

			b32 collided = CheckBlockAndBallCollision(block->halfSize, block->position, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult);
			if (collided)
			{
				blockHitIndex = i;
			}
		}

		if (ballCollisionResult != None)
		{
			if (ballCollisionResult == Top || ballCollisionResult == Bottom)
			{
				ballVelocity.y *= -1.0f;
			}
			else
			{
				ballVelocity.x *= -1.0f;
			}

			if (blockHitIndex != -1)
			{
				Block *block = &blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
				block->exists = false;
			}
		}

		// bat
		ClearScreenAndDrawRect(&renderBuffer, BAT_COLOR, BACKGROUND_COLOR, playerHalfSize, playerPosition);

		// blocks
		for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
		{
			if (!block->exists) continue;

			DrawRect(&renderBuffer, block->color, block->halfSize, block->position);
		}

		// bat
		DrawRect(&renderBuffer, BALL_COLOR, ballHalfSize, ballPosition);
	}
}