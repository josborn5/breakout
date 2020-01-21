#include "math.h"

const float INIT_BALL_SPEED = 100.0f;
const float MAX_BALL_SPEED = 1000.0f;
const float MIN_BALL_SPEED = 50.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;

const float BLOCK_WIDTH = 6.0f;
const float BLOCK_HEIGHT = 3.0f;

const float BALL_SIZE = 2.0f;

const float BAT_WIDTH = 10.0f;
const float BAT_HEIGHT = 1.0f;

Rect GAME_RECT;

internal const int X_DIM_BASE = 160;
internal const int Y_DIM_BASE = 90;

Vector2D prevBallPosition;
Vector2D ballPosition;
Vector2D ballVelocity;
Vector2D ballHalfSize;

float minPlayerX;
float maxPlayerX;

Vector2D prevPlayerPosition;
Vector2D playerPosition;
Vector2D playerVelocity;
Vector2D playerHalfSize;

Vector2D worldPosition;
Vector2D worldHalfSize;

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
	Rect pixelRect = (Rect) { renderBuffer.width, renderBuffer.height };
	if (!initialized)
	{
		initialized = true;
		GAME_RECT = (Rect) { X_DIM_BASE, Y_DIM_BASE, ((float)X_DIM_BASE / (float)Y_DIM_BASE) };
		float worldHalfX = 0.5f * (float)X_DIM_BASE;
		float worldHalfY = 0.5f * (float)Y_DIM_BASE;
		worldHalfSize = (Vector2D){ worldHalfX, worldHalfY };
		worldPosition = (Vector2D){ worldHalfX, worldHalfY };
		ballVelocity.y = 50;
		ballVelocity.x = 5;

		ballPosition.y = 20 + ballHalfSize.y;
		ballPosition.x = 20 + ballHalfSize.x;

		ballHalfSize = (Vector2D){ BALL_SIZE, BALL_SIZE };

		minPlayerX = playerHalfSize.x;
		maxPlayerX = X_DIM_BASE - playerHalfSize.x;

		playerPosition.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
		playerPosition.x = ClampFloat(minPlayerX, playerPosition.x, maxPlayerX);
		playerPosition.y = 20;
		prevPlayerPosition.x = playerPosition.x;
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
				block->position.x = 30 + (2 * x * BLOCK_WIDTH);
				block->position.y = 60 + (2 * y * BLOCK_HEIGHT);
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
		// ball
		prevPlayerPosition.x = playerPosition.x;
		playerPosition.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
		playerPosition.x = ClampFloat(0, playerPosition.x, X_DIM_BASE);
		playerVelocity.x = (playerPosition.x - prevPlayerPosition.x) / dt;

		float minCollisionTime = dt;
		float t1 = dt;
		float t0 = 0.0f;
		int ballCollisionResult = None;
		int collisionCheckCount = 0;
		int maxCollisionCheckCount = 4;
		b32 checkCollision = true;

		while(checkCollision && collisionCheckCount < maxCollisionCheckCount)
		{
			// cache the previous position
			prevBallPosition.x = ballPosition.x;
			prevBallPosition.y = ballPosition.y;
			
			ballPosition = AddVector2D(prevBallPosition, MultiplyVector2D(ballVelocity, t1 - t0));

			// Define a rect over the path the ball takes in the current timestep. Use this to work our which blocks could be collided with
			Vector2D ballPathBottomLeft = (Vector2D) {MinFloat(ballPosition.x, prevBallPosition.x), MinFloat(ballPosition.y, prevBallPosition.y)};
			Vector2D ballPathTopRight = (Vector2D) {MaxFloat(ballPosition.x, prevBallPosition.x), MaxFloat(ballPosition.y, prevBallPosition.y)};

			// Check for collision between any boundary of the world
			CheckBlockAndTopsideOfWallCollision(0, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);
			CheckBlockAndUndersideOfWallCollision(Y_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);
			CheckBlockAndLeftWallCollision(0, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);
			CheckBlockAndRightWallCollision(X_DIM_BASE, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);

			int blockHitIndex = -1;

			// check for collision between ball and blocks
			for (int i = 0; i < ArrayCount(blocks); i++)
			// for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
			{
				Block *block = &blocks[i];
				if (!block->exists) continue;

				// Check if the block lies on the path the ball takes on the current timestep
				// if(!AABBCollideCornerToRect(block->halfSize, block->position, ballPathTopRight, ballPathBottomLeft)) continue;

				b32 collided = CheckBlockAndBallCollision(block->halfSize, block->position, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);
				if (collided)
				{
					blockHitIndex = i;
				}
			}

			// Check for collision between ball and bat
			b32 playerCollision = CheckCollisionBetweenMovingObjects(playerHalfSize, prevPlayerPosition, playerVelocity, ballHalfSize, prevBallPosition, ballVelocity, &minCollisionTime, &ballCollisionResult, &ballPosition);

			checkCollision = (ballCollisionResult != None);
			if (checkCollision)
			{
				if (ballCollisionResult == Top || ballCollisionResult == Bottom)
				{
					ballVelocity.y *= -1.0f;
				}
				else
				{
					if (playerCollision)
					{
						if (ballVelocity.x > 0 && playerVelocity.x > 0
							|| ballVelocity.x < 0 && playerVelocity.x < 0)
						{
							ballVelocity.x = MaxFloat(playerVelocity.x, ballVelocity.x);
						}
						else
						{
							ballVelocity.x *= -1.0f;
						}
					}
					else
					{
						ballVelocity.x *= -1.0f;
					}
				}
				t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
				ballCollisionResult = None;	// Reset collision result for next loop

				if (blockHitIndex != -1)
				{
					Block *block = &blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
					block->exists = false;
				}
			}
			collisionCheckCount += 1;
		}

		// add some air resistance so ball slows down to normal speed after a while
		if (ballVelocity.x > 0)
		{
			ballVelocity.x = MaxFloat(MIN_BALL_SPEED, ballVelocity.x *= 0.999);
		}
		else if (ballVelocity.x < 0)
		{
			ballVelocity.x = MinFloat(-MIN_BALL_SPEED, ballVelocity.x *= 0.999);
		}
	}

	// background
	ClearScreenAndDrawRect(&renderBuffer, GAME_RECT, BACKGROUND_COLOR, 0x000000, worldHalfSize, worldPosition);

	// player
	DrawRect(&renderBuffer, GAME_RECT, BAT_COLOR, playerHalfSize, playerPosition);

	// blocks
	for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
	{
		if (!block->exists) continue;

		DrawRect(&renderBuffer, GAME_RECT, block->color, block->halfSize, block->position);
	}

	// ball
	DrawRect(&renderBuffer, GAME_RECT, BALL_COLOR, ballHalfSize, ballPosition);
}