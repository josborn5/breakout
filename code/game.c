#include "math.h"
#include "game.h"
#include "platform.h"

#define BLOCK_ARRAY_SIZE 64
#define BLOCK_AREA (Vector2D){ 100.0f, 20.0f }
#define BLOCK_AREA_POS (Vector2D){ 30.0f, 70.0f }

const float MIN_BALL_SPEED = 75.0f;
const float LEVEL_CHANGE_BALL_SPEED = 5.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

const int BLOCK_SCORE = 10;

const float BLOCK_WIDTH = 6.0f;
const float BLOCK_HEIGHT = 3.0f;
const float FONT_SIZE = 2.0f;
const float BALL_SIZE = 1.0f;
const float BAT_WIDTH = 10.0f;
const float BAT_HEIGHT = 1.0f;

Rect GAME_RECT;

internal const int X_DIM_BASE = 160;
internal const int Y_DIM_BASE = 90;
const int STARTING_LIVES = 3;

float minPlayerX;
float maxPlayerX;

Vector2D worldPosition;
Vector2D worldHalfSize;

Player player;
Ball ball;
Block blocks[BLOCK_ARRAY_SIZE];

b32 initialized = false;
b32 isPaused = false;
b32 allBlocksCleared = false;;
int level;
int score;
int lives;

char debugStringBuffer[256];

static void ResetBall()
{
	ball.velocity.y = MIN_BALL_SPEED;
	ball.velocity.x = MIN_BALL_SPEED;

	ball.position.y = 20 + ball.halfSize.y;
	ball.position.x = 20 + ball.halfSize.x;
}

static void StartLevel(char newLevel)
{
	level = newLevel;
	allBlocksCleared = false;

	ResetBall();

	PopulateBlocksForLevel(newLevel, blocks, BLOCK_ARRAY_SIZE, BLOCK_AREA, BLOCK_AREA_POS);
}

static float GetThetaForBallPlayerCollision(float playerPositionX, float ballPositionX, float playerHalfSizeX)
{
	// work out where on the player the ball hit to determine the angle the ball moves after bouncing
	float thetaInRadians = 1.0f;
	float m = thetaInRadians / playerHalfSizeX;
	float b = (thetaInRadians * playerPositionX) / playerHalfSizeX;
	float horFactor = (m * ballPositionX) - b;
	return horFactor;
}

static void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
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
		
		ball.halfSize = (Vector2D){ BALL_SIZE, BALL_SIZE };
		player.halfSize = (Vector2D){ BAT_WIDTH, BAT_HEIGHT };

		minPlayerX = 0.0f;
		maxPlayerX = X_DIM_BASE;

		player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
		player.position.x = ClampFloat(minPlayerX, player.position.x, maxPlayerX);
		player.position.y = 20;
		player.prevPosition.x = player.position.x;
		player.prevPosition.y = player.position.y;
		player.velocity.y = 0;
		player.velocity.x = 0;

		score = 0;
		lives = STARTING_LIVES;
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
		player.prevPosition.x = player.position.x;
		player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
		player.position.x = ClampFloat(minPlayerX, player.position.x, maxPlayerX);
		player.velocity.x = (player.position.x - player.prevPosition.x) / dt;

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
			ball.prevPosition = ball.position;
			ball.position = AddVector2D(ball.prevPosition, MultiplyVector2D(ball.velocity, t1 - t0));

			// Check for collision between any boundary of the world
			CheckBlockAndTopsideOfWallCollision(0, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);
			if (ballCollisionResult != None)
			{
				lives -= 1;

				if (lives <= 0)
				{
					initialized = false; // TODO: GAMEOVER. For now, restart.
					return;
				}
				else
				{
					ResetBall();
					return;
				}
			}

			CheckBlockAndUndersideOfWallCollision(Y_DIM_BASE, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);
			CheckBlockAndLeftWallCollision(0, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);
			CheckBlockAndRightWallCollision(X_DIM_BASE, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);

			int blockHitIndex = -1;

			// check for collision between ball and blocks
			for (int i = 0; i < ArrayCount(blocks); i += 1)
			{
				Block *block = &blocks[i];
				if (!block->exists) continue;

				b32 collided = CheckBlockAndBallCollision(block->halfSize, block->position, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);
				if (collided)
				{
					blockHitIndex = i;
				}
			}

			// Check for collision between ball and bat
			b32 playerCollision = CheckCollisionBetweenMovingObjects(player.halfSize, player.prevPosition, player.velocity, ball.halfSize, ball.prevPosition, ball.velocity, &minCollisionTime, &ballCollisionResult, &ball.position);

			checkCollision = (ballCollisionResult != None);
			if (checkCollision)
			{
				if (ballCollisionResult == Top || ballCollisionResult == Bottom)
				{
					ball.velocity.y *= -1.0f;

					if (playerCollision && ballCollisionResult == Top)
					{
						// Add a horizontal velocity to allow player to change ball direction
						float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, ball.position.x, player.halfSize.x);
						float ballSpeed = GetVectorMagnitude(ball.velocity);
						ball.velocity.x = sin(ballAngleFromNormal) * ballSpeed;
						ball.velocity.y = cos(ballAngleFromNormal) * ballSpeed;
					}
				}
				else
				{
					if (playerCollision)
					{
						if (ballCollisionResult == Left)
						{
							float ballVelocityX = (ball.velocity.x > 0) ? -ball.velocity.x : ball.velocity.x;
							ball.velocity.x = MinFloat(ballVelocityX, player.velocity.x);
						}
						else
						{
							float ballVelocityX = (ball.velocity.x < 0) ? -ball.velocity.x : ball.velocity.x;
							ball.velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
						}
					}
					else // collision with block
					{
						ball.velocity.x *= -1.0f;
					}
				}
				t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
				ballCollisionResult = None;	// Reset collision result for next loop

				if (blockHitIndex != -1)
				{
					Block *block = &blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
					block->exists = false;
					score += BLOCK_SCORE;

					// speed up the ball a little
					ball.velocity.x *= 1.2f;
					ball.velocity.y *= 1.2f;
				}
			}
			collisionCheckCount += 1;
		}

		float minBallSpeed = (allBlocksCleared) ? 0 : MIN_BALL_SPEED;
		// add some air resistance so ball slows down to normal speed after a while
		float ballSpeed = GetVectorMagnitude(ball.velocity);
		if (ballSpeed > minBallSpeed)
		{
			float dSpeed = ballSpeed - (0.995 * ballSpeed);
			float dVelX = dSpeed * ball.velocity.x / ballSpeed;
			float dVelY = dSpeed * ball.velocity.y / ballSpeed;
			ball.velocity.x -= dVelX;
			ball.velocity.y -= dVelY;
		}

		// final bounds check to make sure ball doesn't leave the world
		ball.position.x = ClampFloat(0 + ball.halfSize.x, ball.position.x, X_DIM_BASE - ball.halfSize.x);
		ball.position.y = ClampFloat(0 + ball.halfSize.y, ball.position.y, Y_DIM_BASE - ball.halfSize.y);

		if (allBlocksCleared && (ballSpeed < LEVEL_CHANGE_BALL_SPEED))
		{
			StartLevel(level += 1);
		}
	}

	// background
	ClearScreenAndDrawRect(&renderBuffer, GAME_RECT, BACKGROUND_COLOR, 0x000000, worldHalfSize, worldPosition);

	// player
	DrawRect(&renderBuffer, GAME_RECT, BAT_COLOR, player.halfSize, player.position);

	// blocks
	allBlocksCleared = true;
	for (Block *block = blocks; block != blocks + ArrayCount(blocks); block++)
	{
		if (!block->exists) continue;

		allBlocksCleared = false;
		DrawRect(&renderBuffer, GAME_RECT, block->color, block->halfSize, block->position);
	}

	// ball
	DrawRect(&renderBuffer, GAME_RECT, BALL_COLOR, ball.halfSize, ball.position);

	// Balls, Level & Score
	DrawAlphabetCharacters(&renderBuffer, GAME_RECT, "BALLS", (Vector2D){ 10.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(&renderBuffer, GAME_RECT, lives, (Vector2D){ 25.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(&renderBuffer, GAME_RECT, "LEVEL", (Vector2D){ 65.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(&renderBuffer, GAME_RECT, level, (Vector2D){ 80.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(&renderBuffer, GAME_RECT, "SCORE", (Vector2D){ 120.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(&renderBuffer, GAME_RECT, score, (Vector2D){ 135.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
}