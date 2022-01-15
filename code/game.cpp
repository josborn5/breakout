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
#include "../../win32-platform/bin/platform.hpp"
#include "../../win32-platform/bin/game.hpp"
#include "utils.h"
#include "../../win32-platform/bin/math.hpp"
#include "../../win32-platform/bin/collision.hpp"

#include "math.c"
#include "collision.c"
#include "world_transforms.c"
#include "software_rendering.c"
#include "platform_common.c"
#include "levels.c"
#include "main_win32.cpp"

#define BLOCK_AREA Vector2D { 100.0f, 20.0f }
#define BLOCK_AREA_POS Vector2D { 30.0f, 70.0f }
#define BALL_HALF_SIZE Vector2D { 1.0f, 1.0f }

const float MIN_BALL_SPEED = 40.0f;
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

Rect GAME_RECT = {0};

const int X_DIM_ORIGIN = 0;
const int X_DIM_BASE = 160;
const int Y_DIM_ORIGIN = 0;
const int Y_DIM_BASE = 90;
const int STARTING_LIVES = 3;

float minPlayerX;
float maxPlayerX;

Vector2D worldPosition = {0};
Vector2D worldHalfSize = {0};

GameState gamestate = {0};

bool initialized = false;
bool isPaused = false;
bool allBlocksCleared = false;;

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

static void StartLevel(int newLevel)
{
	allBlocksCleared = false;

	gamestate.isCometActive = false;

	ResetBalls();

	PopulateBlocksForLevel(newLevel, gamestate.blocks, BLOCK_ARRAY_SIZE, BLOCK_AREA, BLOCK_AREA_POS);
}

static void InitializeGameState(GameState *state, Rect pixelRect, const Input &input)
{
	GAME_RECT.x = X_DIM_BASE;
	GAME_RECT.y = Y_DIM_BASE;
	GAME_RECT.aspectRatio = ((float)X_DIM_BASE) / ((float)Y_DIM_BASE);

	float worldHalfX = 0.5f * (float)X_DIM_BASE;
	float worldHalfY = 0.5f * (float)Y_DIM_BASE;
	worldHalfSize.x = worldHalfX;
	worldHalfSize.y = worldHalfY;
	worldPosition.x = worldHalfX;
	worldPosition.y = worldHalfY;

	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		state->balls[i].halfSize = BALL_HALF_SIZE;
	}

	state->player.halfSize.x = BAT_WIDTH;
	state->player.halfSize.y = BAT_HEIGHT;

	minPlayerX = 0.0f;
	maxPlayerX = (float)X_DIM_BASE;

	state->player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input.mouse.x, input.mouse.y).x;
	state->player.position.x = ClampFloat(minPlayerX, state->player.position.x, maxPlayerX);
	state->player.position.y = 20;
	state->player.prevPosition = state->player.position;
	state->player.velocity = gentle::Vec2<float> { 0.0f, 0.0f };

	state->score = 0;
	state->lives = STARTING_LIVES;
	state->level = 1;
	StartLevel(state->level);
}

static void UpdateGameState(GameState *state, Rect pixelRect, const Input &input, float dt)
{
	// Update player state
	state->player.prevPosition.x = state->player.position.x;
	state->player.position.x = TransformPixelCoordToGameCoord(pixelRect, GAME_RECT, input.mouse.x, input.mouse.y).x;
	state->player.position.x = ClampFloat(minPlayerX, state->player.position.x, maxPlayerX);
	state->player.velocity.x = (state->player.position.x - state->player.prevPosition.x) / dt;

	// Update ball & block state
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!state->balls[i].exists) continue;
		
		float minCollisionTime = dt;
		float t1 = dt;
		float t0 = 0.0f;
		int ballCollisionResult = None;
		int collisionCheckCount = 0;
		int maxCollisionCheckCount = 4;
		bool checkCollision = true;

		while(checkCollision && collisionCheckCount < maxCollisionCheckCount)
		{
			// cache the previous position
			state->balls[i].prevPosition = state->balls[i].position;
			state->balls[i].position = AddVector2D(state->balls[i].prevPosition, MultiplyVector2D(state->balls[i].velocity, t1 - t0));

			// Check for collision between any boundary of the world
			CheckBlockAndTopsideOfWallCollision((float)Y_DIM_ORIGIN, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);
			if (ballCollisionResult != None)
			{
				state->balls[i].exists = false;

				bool anyBallsLeft = false;
				for (int j = 0; j < BALL_ARRAY_SIZE; j += 1)
				{
					if (state->balls[j].exists)
					{
						anyBallsLeft = true;
					}
				}

				if (!anyBallsLeft)
				{
					state->lives -= 1;

					if (state->lives <= 0)
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

			CheckBlockAndUndersideOfWallCollision((float)Y_DIM_BASE, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);
			CheckBlockAndLeftWallCollision((float)X_DIM_ORIGIN, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);
			CheckBlockAndRightWallCollision((float)X_DIM_BASE, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);

			int blockHitIndex = NO_BLOCK_HIT_INDEX;

			// check for collision between ball and blocks
			for (int j = 0; j < ArrayCount(state->blocks); j += 1)
			{
				Block *block = &state->blocks[j];
				if (!block->exists) continue;

				bool collided = CheckBlockAndBallCollision(block->halfSize, block->position, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);
				if (collided)
				{
					blockHitIndex = j;
				}
			}

			// Check for collision between ball and bat
			Player player = state->player;
			Vector2D tempPlayerPrevPosition = Vector2D { player.prevPosition.x, player.prevPosition.y };
			Vector2D tempPlayerVelocity = Vector2D { player.velocity.x, player.velocity.y };
			bool playerCollision = CheckCollisionBetweenMovingObjects(player.halfSize, tempPlayerPrevPosition, tempPlayerVelocity, state->balls[i].halfSize, state->balls[i].prevPosition, state->balls[i].velocity, &minCollisionTime, &ballCollisionResult, &state->balls[i].position);
			bool blockCollision = blockHitIndex != NO_BLOCK_HIT_INDEX;

			checkCollision = (ballCollisionResult != None);
			if (checkCollision)
			{
				if (ballCollisionResult == Top || ballCollisionResult == Bottom)
				{
					if (!blockCollision || !state->isCometActive)
					{
						state->balls[i].velocity.y *= -1.0f;
					}

					if (playerCollision && ballCollisionResult == Top)
					{
						// Add a horizontal velocity to allow player to change ball direction
						float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, state->balls[i].position.x, player.halfSize.x);
						float ballSpeed = GetVectorMagnitude(state->balls[i].velocity);
						state->balls[i].velocity.x = (float)sin(ballAngleFromNormal) * ballSpeed;
						state->balls[i].velocity.y = (float)cos(ballAngleFromNormal) * ballSpeed;
					}
				}
				else
				{
					if (playerCollision)
					{
						if (ballCollisionResult == Left)
						{
							float ballVelocityX = (state->balls[i].velocity.x > 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
							state->balls[i].velocity.x = MinFloat(ballVelocityX, player.velocity.x);
						}
						else
						{
							float ballVelocityX = (state->balls[i].velocity.x < 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
							state->balls[i].velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
						}
					}
					else // collision with block or wall
					{
						if (!blockCollision || !state->isCometActive)
						{
							state->balls[i].velocity.x *= -1.0f;
						}
					}
				}
				t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
				ballCollisionResult = None;	// Reset collision result for next loop

				if (blockHitIndex != -1)
				{
					Block *block = &state->blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
					block->exists = false;
					state->score += BLOCK_SCORE;

					// speed up the ball a little
					//gamestate->balls[i].velocity.x *= 1.2f;
					//gamestate->balls[i].velocity.y *= 1.2f;

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
		float ballSpeed = GetVectorMagnitude(state->balls[i].velocity);
		if (ballSpeed > minBallSpeed)
		{
			float dSpeed = ballSpeed - (0.995f * ballSpeed);
			float dVelX = dSpeed * state->balls[i].velocity.x / ballSpeed;
			float dVelY = dSpeed * state->balls[i].velocity.y / ballSpeed;
			state->balls[i].velocity.x -= dVelX;
			state->balls[i].velocity.y -= dVelY;
		}

		// final bounds check to make sure ball doesn't leave the world
		state->balls[i].position.x = ClampFloat(0 + state->balls[i].halfSize.x, state->balls[i].position.x, X_DIM_BASE - state->balls[i].halfSize.x);
		state->balls[i].position.y = ClampFloat(0 + state->balls[i].halfSize.y, state->balls[i].position.y, Y_DIM_BASE - state->balls[i].halfSize.y);
	}

	// Update power up state
	for (Block *block = state->blocks; block != state->blocks + ArrayCount(state->blocks); block++)
	{
		if (!block->powerUp.exists) continue;

		block->powerUp.prevPosition = block->powerUp.position;
		block->powerUp.position = gentle::AddVectors(block->powerUp.prevPosition, gentle::MultiplyVectorByScalar(block->powerUp.velocity, dt));

		// Can get away with a super simple position check for the power up falling off screen here
		if (block->powerUp.position.y < Y_DIM_ORIGIN)
		{
			block->powerUp.exists = false;
		}
		else
		{
			float minCollisionTime = dt;
			gentle::CollisionSide ballCollisionResult = gentle::None;
			gentle::Vec2<float> tempPowerUpHalfSize = gentle::Vec2<float> { block->powerUp.halfSize.x, block->powerUp.halfSize.y };
			gentle::Vec2<float> tempPlayerHalfSize = gentle::Vec2<float> { state->player.halfSize.x, state->player.halfSize.y };
			gentle::CheckCollisionBetweenMovingRects(
				tempPlayerHalfSize,
				state->player.prevPosition,
				state->player.velocity,
				tempPowerUpHalfSize,
				block->powerUp.prevPosition,
				block->powerUp.velocity,
				&minCollisionTime,
				&ballCollisionResult,
				&block->powerUp.position
			);

			if (ballCollisionResult != None)
			{
				block->powerUp.exists = false;

				switch (block->powerUp.type)
				{
					case Comet:
						state->isCometActive = true;
						break;
					case Multiball:
						// get the first ball that exists
						Ball* existingBall = state->balls;
						while (!existingBall->exists)
						{
							existingBall++;
						}

						for (int j = 0; j < BALL_ARRAY_SIZE; j += 1)
						{
							Ball* ball = &state->balls[j];
							if (ball->exists) continue;

							ball->exists = true;
							ball->position = existingBall->position;
							ball->velocity.y = existingBall->velocity.y;
							ball->velocity.x = existingBall->velocity.x + (j * 10.0f);
						}
						break;
				}
			}
		}
	}

	if (allBlocksCleared && (GetVectorMagnitude(state->balls[0].velocity) < LEVEL_CHANGE_BALL_SPEED))
	{
		state->level += 1;
		StartLevel(state->level);
	}
}

static void RenderGameState(const RenderBuffer &renderBuffer, const GameState &state)
{
	// background
	ClearScreenAndDrawRect(renderBuffer, GAME_RECT, BACKGROUND_COLOR, 0x000000, worldHalfSize, worldPosition);

	// player
	Vector2D tempPlayerPosition = Vector2D { state.player.position.x, state.player.position.y };
	DrawRect(renderBuffer, GAME_RECT, BAT_COLOR, state.player.halfSize, tempPlayerPosition);

	// blocks
	allBlocksCleared = true;
	for (const Block *block = state.blocks; block != state.blocks + ArrayCount(state.blocks); block++)
	{
		if (!block->exists) continue;

		allBlocksCleared = false;
		DrawRect(renderBuffer, GAME_RECT, block->color, block->halfSize, block->position);
	}

	// ball
	for (int i = 0; i < BALL_ARRAY_SIZE; i += 1)
	{
		if (!state.balls[i].exists) continue;

		DrawRect(renderBuffer, GAME_RECT, BALL_COLOR, state.balls[i].halfSize, state.balls[i].position);
	}

	// power ups
	for (const Block *block = state.blocks; block != state.blocks + ArrayCount(state.blocks); block++)
	{
		if (!block->powerUp.exists) continue;

		Vector2D tempPowerUpPosition = Vector2D { block->powerUp.position.x, block->powerUp.position.y };
		DrawRect(renderBuffer, GAME_RECT, block->powerUp.color, block->powerUp.halfSize, tempPowerUpPosition);
	}

	// Balls, Level & Score
	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "BALLS", Vector2D { 10.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.lives, Vector2D { 25.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "LEVEL", Vector2D { 65.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.level, Vector2D { 80.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "SCORE", Vector2D { 120.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.score, Vector2D { 135.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
}

void gentle::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{

}

void gentle::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	Rect pixelRect = {0};
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;

	if (IsReleased(input, KEY_R))
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		InitializeGameState(&gamestate, pixelRect, input);
		return;
	}

	if (IsReleased(input, KEY_SPACE))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, pixelRect, input, dt);
	}

	RenderGameState(renderBuffer, gamestate);
}