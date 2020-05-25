/*

TODO (in no particular order):

-- Multi-ball power up
-- Visual effects for power ups

-- Clean up the ball speed slow down logic. We're doing a lot of operations here that I think could be simplified.

-- Support defining colors in the DrawSprite function. Could use a char value to indicate a color. Need to work out how to intuitively slice the color spectrum into A-Z, a-z, 0-9 characters.

-- Render the player, ball and blocks as sprites instead of rects.

-- Measure processing times for each frame. Measure processing times for the game state update & rendering steps of each frame.

-- Once measuring is in place, see where optimization is needed.

-- Intro screen and inter-level screen

-- Sound! Check out the JavidX9 youtube videos for some ideas on how to implement this.

-- More levels!

*/

#include "math.h"
#include "game.h"
#include "platform.h"

#define BLOCK_AREA (Vector2D){ 100.0f, 20.0f }
#define BLOCK_AREA_POS (Vector2D){ 30.0f, 70.0f }
#define BALL_HALF_SIZE (Vector2D){ 1.0f, 1.0f }

const float MIN_BALL_SPEED = 75.0f;
const float LEVEL_CHANGE_BALL_SPEED = 5.0f;

const uint32_t BACKGROUND_COLOR = 0x551100;
const uint32_t BALL_COLOR = 0x0000FF;
const uint32_t BAT_COLOR = 0x00FF00;
const uint32_t BLOCK_COLOR = 0xFFFF00;
const uint32_t TEXT_COLOR = 0xFFFF00;

const int BLOCK_SCORE = 10;
const int NO_BLOCK_HIT_INDEX = -1;

const float BLOCK_WIDTH = 6.0f;
const float BLOCK_HEIGHT = 3.0f;
const float FONT_SIZE = 2.0f;
const float BAT_WIDTH = 10.0f;
const float BAT_HEIGHT = 1.0f;

Rect GAME_RECT;

internal const int X_DIM_ORIGIN = 0;
internal const int X_DIM_BASE = 160;
internal const int Y_DIM_ORIGIN = 0;
internal const int Y_DIM_BASE = 90;
const int STARTING_LIVES = 3;

float minPlayerX;
float maxPlayerX;

Vector2D worldPosition;
Vector2D worldHalfSize;

GameState gamestate;

b32 initialized = false;
b32 isPaused = false;
b32 allBlocksCleared = false;;

char debugStringBuffer[256];

static void ResetBalls()
{
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		gamestate.balls[i].exists = (i == 0);
		gamestate.balls[i].velocity.y = MIN_BALL_SPEED;
		gamestate.balls[i].velocity.x = MIN_BALL_SPEED;

		gamestate.balls[i].position.y = 20 + gamestate.balls[i].halfSize.y;
		gamestate.balls[i].position.x = 20 + gamestate.balls[i].halfSize.x;
	}
}

static void StartLevel(char newLevel)
{
	allBlocksCleared = false;

	gamestate.isCometActive = false;

	ResetBalls();

	PopulateBlocksForLevel(newLevel, gamestate.blocks, BLOCK_ARRAY_SIZE, BLOCK_AREA, BLOCK_AREA_POS);
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

static void InitializeGameState(GameState *gamestate, Rect pixelRect, Input *input)
{
	GAME_RECT = (Rect) { X_DIM_BASE, Y_DIM_BASE, ((float)X_DIM_BASE / (float)Y_DIM_BASE) };
	float worldHalfX = 0.5f * (float)X_DIM_BASE;
	float worldHalfY = 0.5f * (float)Y_DIM_BASE;
	worldHalfSize = (Vector2D){ worldHalfX, worldHalfY };
	worldPosition = (Vector2D){ worldHalfX, worldHalfY };

	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		gamestate->balls[i].halfSize = BALL_HALF_SIZE;
	}

	gamestate->player.halfSize = (Vector2D){ BAT_WIDTH, BAT_HEIGHT };

	minPlayerX = 0.0f;
	maxPlayerX = X_DIM_BASE;

	gamestate->player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
	gamestate->player.position.x = ClampFloat(minPlayerX, gamestate->player.position.x, maxPlayerX);
	gamestate->player.position.y = 20;
	gamestate->player.prevPosition = gamestate->player.position;
	gamestate->player.velocity = ZERO_VECTOR;

	gamestate->score = 0;
	gamestate->lives = STARTING_LIVES;
	gamestate->level = 1;
	StartLevel(gamestate->level);
}

static void UpdateGameState(GameState *gamestate, Rect pixelRect, Input *input, float dt)
{
	// Update player state
	gamestate->player.prevPosition.x = gamestate->player.position.x;
	gamestate->player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input->mouse.x, input->mouse.y).x;
	gamestate->player.position.x = ClampFloat(minPlayerX, gamestate->player.position.x, maxPlayerX);
	gamestate->player.velocity.x = (gamestate->player.position.x - gamestate->player.prevPosition.x) / dt;

	// Update ball & block state
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!gamestate->balls[i].exists) continue;
		
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
			gamestate->balls[i].prevPosition = gamestate->balls[i].position;
			gamestate->balls[i].position = AddVector2D(gamestate->balls[i].prevPosition, MultiplyVector2D(gamestate->balls[i].velocity, t1 - t0));

			// Check for collision between any boundary of the world
			CheckBlockAndTopsideOfWallCollision(Y_DIM_ORIGIN, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);
			if (ballCollisionResult != None)
			{
				gamestate->balls[i].exists = false;

				b32 anyBallsLeft = false;
				for (int j = 0; j < BALL_ARRAY_SIZE; j += 1)
				{
					if (gamestate->balls[j].exists)
					{
						anyBallsLeft = true;
					}
				}

				if (!anyBallsLeft)
				{
					gamestate->lives -= 1;

					if (gamestate->lives <= 0)
					{
						initialized = false; // TODO: GAMEOVER. For now, restart.
						return;
					}
					else
					{
						ResetBalls();
						return;
					}
				}
			}

			CheckBlockAndUndersideOfWallCollision(Y_DIM_BASE, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);
			CheckBlockAndLeftWallCollision(X_DIM_ORIGIN, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);
			CheckBlockAndRightWallCollision(X_DIM_BASE, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);

			int blockHitIndex = NO_BLOCK_HIT_INDEX;

			// check for collision between ball and blocks
			for (int j = 0; j < ArrayCount(gamestate->blocks); j += 1)
			{
				Block *block = &gamestate->blocks[j];
				if (!block->exists) continue;

				b32 collided = CheckBlockAndBallCollision(block->halfSize, block->position, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);
				if (collided)
				{
					blockHitIndex = j;
				}
			}

			// Check for collision between ball and bat
			Player player = gamestate->player;
			b32 playerCollision = CheckCollisionBetweenMovingObjects(player.halfSize, player.prevPosition, player.velocity, gamestate->balls[i].halfSize, gamestate->balls[i].prevPosition, gamestate->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &gamestate->balls[i].position);
			b32 blockCollision = blockHitIndex != NO_BLOCK_HIT_INDEX;

			checkCollision = (ballCollisionResult != None);
			if (checkCollision)
			{
				if (ballCollisionResult == Top || ballCollisionResult == Bottom)
				{
					if (!blockCollision || !gamestate->isCometActive)
					{
						gamestate->balls[i].velocity.y *= -1.0f;
					}

					if (playerCollision && ballCollisionResult == Top)
					{
						// Add a horizontal velocity to allow player to change ball direction
						float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, gamestate->balls[i].position.x, player.halfSize.x);
						float ballSpeed = GetVectorMagnitude(gamestate->balls[i].velocity);
						gamestate->balls[i].velocity.x = sin(ballAngleFromNormal) * ballSpeed;
						gamestate->balls[i].velocity.y = cos(ballAngleFromNormal) * ballSpeed;
					}
				}
				else
				{
					if (playerCollision)
					{
						if (ballCollisionResult == Left)
						{
							float ballVelocityX = (gamestate->balls[i].velocity.x > 0) ? -gamestate->balls[i].velocity.x : gamestate->balls[i].velocity.x;
							gamestate->balls[i].velocity.x = MinFloat(ballVelocityX, player.velocity.x);
						}
						else
						{
							float ballVelocityX = (gamestate->balls[i].velocity.x < 0) ? -gamestate->balls[i].velocity.x : gamestate->balls[i].velocity.x;
							gamestate->balls[i].velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
						}
					}
					else // collision with block or wall
					{
						if (!blockCollision || !gamestate->isCometActive)
						{
							gamestate->balls[i].velocity.x *= -1.0f;
						}
					}
				}
				t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
				ballCollisionResult = None;	// Reset collision result for next loop

				if (blockHitIndex != -1)
				{
					Block *block = &gamestate->blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
					block->exists = false;
					gamestate->score += BLOCK_SCORE;

					// speed up the ball a little
					gamestate->balls[i].velocity.x *= 1.2f;
					gamestate->balls[i].velocity.y *= 1.2f;

					// Check for powerup
					if (block->powerUp.type != Nothing)
					{
						block->powerUp.exists = true;
					}
				}
			}
			collisionCheckCount += 1;
		}

		float minBallSpeed = (allBlocksCleared) ? 0 : MIN_BALL_SPEED;
		// add some air resistance so ball slows down to normal speed after a while
		float ballSpeed = GetVectorMagnitude(gamestate->balls[i].velocity);
		if (ballSpeed > minBallSpeed)
		{
			float dSpeed = ballSpeed - (0.995 * ballSpeed);
			float dVelX = dSpeed * gamestate->balls[i].velocity.x / ballSpeed;
			float dVelY = dSpeed * gamestate->balls[i].velocity.y / ballSpeed;
			gamestate->balls[i].velocity.x -= dVelX;
			gamestate->balls[i].velocity.y -= dVelY;
		}

		// final bounds check to make sure ball doesn't leave the world
		gamestate->balls[i].position.x = ClampFloat(0 + gamestate->balls[i].halfSize.x, gamestate->balls[i].position.x, X_DIM_BASE - gamestate->balls[i].halfSize.x);
		gamestate->balls[i].position.y = ClampFloat(0 + gamestate->balls[i].halfSize.y, gamestate->balls[i].position.y, Y_DIM_BASE - gamestate->balls[i].halfSize.y);

	}

	// Update power up state
	for (Block *block = gamestate->blocks; block != gamestate->blocks + ArrayCount(gamestate->blocks); block++)
	{
		if (!block->powerUp.exists) continue;

		block->powerUp.prevPosition = block->powerUp.position;
		block->powerUp.position = AddVector2D(block->powerUp.prevPosition, MultiplyVector2D(block->powerUp.velocity, dt));

		// Can get away with a super simple position check for the power up falling off screen here
		if (block->powerUp.position.y < Y_DIM_ORIGIN)
		{
			block->powerUp.exists = false;
		}
		else
		{
			float minCollisionTime = dt;
			int ballCollisionResult = None;
			CheckCollisionBetweenMovingObjects(gamestate->player.halfSize, gamestate->player.prevPosition, gamestate->player.velocity, block->powerUp.halfSize, block->powerUp.prevPosition, block->powerUp.velocity, &minCollisionTime, &ballCollisionResult, &block->powerUp.position);
			if (ballCollisionResult != None)
			{
				block->powerUp.exists = false;

				switch (block->powerUp.type)
				{
					case Comet:
						gamestate->isCometActive = true;
						break;
					case Multiball:
						// TODO fill up any non-existing balls in the balls array
						break;
				}
			}
		}
	}

	if (allBlocksCleared && (GetVectorMagnitude(gamestate->balls[0].velocity) < LEVEL_CHANGE_BALL_SPEED))
	{
		gamestate->level += 1;
		StartLevel(gamestate->level);
	}
}

static void RenderGameState(RenderBuffer *renderBuffer, GameState *gamestate)
{
	// background
	ClearScreenAndDrawRect(renderBuffer, GAME_RECT, BACKGROUND_COLOR, 0x000000, worldHalfSize, worldPosition);

	// player
	DrawRect(renderBuffer, GAME_RECT, BAT_COLOR, gamestate->player.halfSize, gamestate->player.position);

	// blocks
	allBlocksCleared = true;
	for (Block *block = gamestate->blocks; block != gamestate->blocks + ArrayCount(gamestate->blocks); block++)
	{
		if (!block->exists) continue;

		allBlocksCleared = false;
		DrawRect(renderBuffer, GAME_RECT, block->color, block->halfSize, block->position);
	}

	// ball
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!gamestate->balls[i].exists) continue;

		DrawRect(renderBuffer, GAME_RECT, BALL_COLOR, gamestate->balls[i].halfSize, gamestate->balls[i].position);
	}

	// power ups
	for (Block *block = gamestate->blocks; block != gamestate->blocks + ArrayCount(gamestate->blocks); block++)
	{
		if (!block->powerUp.exists) continue;

		DrawRect(renderBuffer, GAME_RECT, block->powerUp.color, block->powerUp.halfSize, block->powerUp.position);
	}

	// Balls, Level & Score
	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "BALLS", (Vector2D){ 10.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, gamestate->lives, (Vector2D){ 25.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "LEVEL", (Vector2D){ 65.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, gamestate->level, (Vector2D){ 80.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "SCORE", (Vector2D){ 120.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, gamestate->score, (Vector2D){ 135.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
}

static void SimulateGame(Input *input, RenderBuffer renderBuffer, float dt)
{
	Rect pixelRect = (Rect) { renderBuffer.width, renderBuffer.height };

	if (IsReleased(input, BUTTON_RESET))
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		InitializeGameState(&gamestate, pixelRect, input);
		return;
	}

	if (IsReleased(input, BUTTON_PAUSE))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, pixelRect, input, dt);
	}

	RenderGameState(&renderBuffer, &gamestate);
}