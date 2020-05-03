#include "math.h"
#include "game.h"
#include "platform.h"

#define BLOCK_ARRAY_SIZE 64
#define BLOCK_AREA (Vector2D){ 100.0f, 20.0f }
#define BLOCK_AREA_POS (Vector2D){ 30.0f, 70.0f }

const float MIN_BALL_SPEED = 85.0f;
const float LEVEL_CHANGE_BALL_SPEED = 5.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

const float BLOCK_WIDTH = 6.0f;
const float BLOCK_HEIGHT = 3.0f;

const int BLOCK_SCORE = 10;

const float BALL_SIZE = 1.0f;

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

Block blocks[BLOCK_ARRAY_SIZE];
int nextBlock;

b32 initialized = false;
b32 isPaused = false;
b32 allBlocksCleared = false;;
int level;
int score;

char debugStringBuffer[256];

static void StartLevel(char newLevel)
{
	level = newLevel;
	allBlocksCleared = false;
	ballVelocity.y = MIN_BALL_SPEED;
	ballVelocity.x = MIN_BALL_SPEED;

	ballPosition.y = 20 + ballHalfSize.y;
	ballPosition.x = 20 + ballHalfSize.x;

	PopulateBlocksForLevel(newLevel, blocks, BLOCK_ARRAY_SIZE, BLOCK_AREA, BLOCK_AREA_POS);
}

internal void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	if (IsReleased(input, BUTTON_RESET))
	{
		initialized = false;
	}

	Rect pixelRect = (Rect) { renderBuffer.width, renderBuffer.height };
	if (!initialized)
	{
		initialized = true;
		GAME_RECT = (Rect) { X_DIM_BASE, Y_DIM_BASE, ((float)X_DIM_BASE / (float)Y_DIM_BASE) };
		float worldHalfX = 0.5f * (float)X_DIM_BASE;
		float worldHalfY = 0.5f * (float)Y_DIM_BASE;
		worldHalfSize = (Vector2D){ worldHalfX, worldHalfY };
		worldPosition = (Vector2D){ worldHalfX, worldHalfY };
		
		ballHalfSize = (Vector2D){ BALL_SIZE, BALL_SIZE };
		playerHalfSize = (Vector2D){ BAT_WIDTH, BAT_HEIGHT };

		minPlayerX = playerHalfSize.x;
		maxPlayerX = X_DIM_BASE - playerHalfSize.x;

		playerPosition.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
		playerPosition.x = ClampFloat(minPlayerX, playerPosition.x, maxPlayerX);
		playerPosition.y = 20;
		prevPlayerPosition.x = playerPosition.x;
		prevPlayerPosition.y = playerPosition.y;
		playerVelocity.y = 0;
		playerVelocity.x = 0;

		score = 0;
		StartLevel(1);
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
		playerPosition.x = ClampFloat(minPlayerX, playerPosition.x, maxPlayerX);
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
						if (ballCollisionResult == Left)
						{
							float ballVelocityX = (ballVelocity.x > 0) ? -ballVelocity.x : ballVelocity.x;
							ballVelocity.x = MinFloat(ballVelocityX, playerVelocity.x);
						}
						else
						{
							float ballVelocityX = (ballVelocity.x < 0) ? -ballVelocity.x : ballVelocity.x;
							ballVelocity.x = MaxFloat(ballVelocityX, playerVelocity.x);
						}
					}
					else // collision with block
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
					score += BLOCK_SCORE;
				}
			}
			collisionCheckCount += 1;
		}

		float minBallSpeed = (allBlocksCleared) ? 0 : MIN_BALL_SPEED;
		// add some air resistance so ball slows down to normal speed after a while
		float ballSpeed = GetVectorMagnitude(ballVelocity);
		if (ballSpeed > minBallSpeed)
		{
			float dSpeed = ballSpeed - (0.995 * ballSpeed);
			float dVelX = dSpeed * ballVelocity.x / ballSpeed;
			float dVelY = dSpeed * ballVelocity.y / ballSpeed;
			ballVelocity.x -= dVelX;
			ballVelocity.y -= dVelY;
		}

		// final bounds check to make sure ball doesn't leave the world
		ballPosition.x = ClampFloat(0 + ballHalfSize.x, ballPosition.x, X_DIM_BASE - ballHalfSize.x);
		ballPosition.y = ClampFloat(0 + ballHalfSize.y, ballPosition.y, Y_DIM_BASE - ballHalfSize.y);

		if (allBlocksCleared && (ballSpeed < LEVEL_CHANGE_BALL_SPEED))
		{
			StartLevel(level += 1);
		}
	}

	// background
	ClearScreenAndDrawRect(&renderBuffer, GAME_RECT, BACKGROUND_COLOR, 0x000000, worldHalfSize, worldPosition);

	// player
	DrawRect(&renderBuffer, GAME_RECT, BAT_COLOR, playerHalfSize, playerPosition);

	// blocks
	allBlocksCleared = true;
	for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
	{
		if (!block->exists) continue;

		allBlocksCleared = false;
		DrawRect(&renderBuffer, GAME_RECT, block->color, block->halfSize, block->position);
	}

	// ball
	DrawRect(&renderBuffer, GAME_RECT, BALL_COLOR, ballHalfSize, ballPosition);

	// Level & Score
	DrawAlphabetCharacters(&renderBuffer, GAME_RECT, "LEVEL", (Vector2D){ 10.0f, 10.0f}, 2.0f, TEXT_COLOR);
	DrawNumber(&renderBuffer, GAME_RECT, level, (Vector2D){ 25.0f, 10.0f}, 2.0f, TEXT_COLOR);

	DrawAlphabetCharacters(&renderBuffer, GAME_RECT, "SCORE", (Vector2D){ 80.0f, 10.0f}, 2.0f, TEXT_COLOR);
	DrawNumber(&renderBuffer, GAME_RECT, score, (Vector2D){ 95.0f, 10.0f}, 2.0f, TEXT_COLOR);
}