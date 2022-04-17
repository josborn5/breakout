/*

TODO (in no particular order):

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

#include "game.h"
#include "../../win32-platform/bin/platform.hpp"
#include "../../win32-platform/bin/game.hpp"
#include "utils.h"
#include "../../win32-platform/bin/math.hpp"
#include "../../win32-platform/bin/collision.hpp"
#include "../../win32-platform/bin/geometry.hpp"

#include "math.c"
#include "world_transforms.c"
#include "software_rendering.c"
#include "platform_common.c"
#include "levels.c"
#include "main_win32.cpp"

#define BLOCK_AREA gentle::Vec2<float> { 100.0f, 20.0f }
#define BLOCK_AREA_POS gentle::Vec2<float> { 30.0f, 70.0f }
#define BALL_HALF_SIZE gentle::Vec2<float> { 1.0f, 1.0f }

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

const int X_DIM_ORIGIN = 0;
const int X_DIM_BASE = 160;
const int Y_DIM_ORIGIN = 0;
const int Y_DIM_BASE = 90;
const int STARTING_LIVES = 3;

const Boundary topBoundary = { Top, Y_DIM_BASE };
const Boundary bottomBoundary = { Bottom, Y_DIM_ORIGIN };
const Boundary leftBoundary = { Left, X_DIM_ORIGIN };
const Boundary rightBoundary = { Right, X_DIM_BASE };

gentle::Vec2<int> GAME_RECT = { X_DIM_BASE, Y_DIM_BASE };

float minPlayerX;
float maxPlayerX;

gentle::Vec2<float> worldPosition = ZERO_VECTOR;
gentle::Vec2<float> worldHalfSize = ZERO_VECTOR;

GameState gamestate = {0};

bool initialized = false;
bool isPaused = false;
bool allBlocksCleared = false;;

char debugStringBuffer[256];

struct WallCollision
{
	Boundary wall;
	gentle::CollisionResult result;
};

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

static void InitializeGameState(GameState *state, gentle::Vec2<int> pixelRect, const Input &input)
{
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

static WallCollision CheckWallCollision(const Ball &ball, float minimumTime)
{
	float finalCollisionTime = minimumTime;
	WallCollision wallCollision;

	Boundary horizontalWalls[2] = { topBoundary, bottomBoundary };
	for (int i = 0; i < 2; i += 1)
	{
		Boundary wall = horizontalWalls[i];
		gentle::CollisionResult wallCollisionResult = gentle::CheckRectAndXLineCollision(wall.position, ball, finalCollisionTime);
		if (wallCollisionResult.collisions[0].side != gentle::None)
		{
			wallCollision.result = wallCollisionResult;
			wallCollision.wall = wall;
			finalCollisionTime = wallCollisionResult.time;
		}
	}

	Boundary verticalWalls[2] = { leftBoundary, rightBoundary };
	for (int i = 0; i < 2; i += 1)
	{
		Boundary wall = verticalWalls[i];
		gentle::CollisionResult wallCollisionResult = gentle::CheckRectAndYLineCollision(wall.position, ball, finalCollisionTime);
		if (wallCollisionResult.collisions[0].side != gentle::None)
		{
			wallCollision.result = wallCollisionResult;
			wallCollision.wall = wall;
			finalCollisionTime = wallCollisionResult.time;
		}
	}

	return wallCollision;
}

static void UpdateGameState(GameState *state, gentle::Vec2<int> pixelRect, const Input &input, float dt)
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
		int collisionCheckCount = 0;
		int maxCollisionCheckCount = 4;
		bool checkCollision = true;

		while(checkCollision && collisionCheckCount < maxCollisionCheckCount)
		{
			// Check for collision between any boundary of the world
			WallCollision ballWallCollision = CheckWallCollision(state->balls[i], minCollisionTime);
			if (ballWallCollision.result.collisions[0].side != gentle::None)
			{
				minCollisionTime = ballWallCollision.result.time;
				checkCollision = true;
			}

			int blockHitIndex = NO_BLOCK_HIT_INDEX;

			gentle::CollisionResult blockBallCollisionResult;
			gentle::CollisionResult hitBlockResult;
			hitBlockResult.collisions[1].side = gentle::None;
			// check for collision between ball and blocks
			for (int j = 0; j < BLOCK_ARRAY_SIZE; j += 1)
			{
				Block block = state->blocks[j];
				if (!block.exists) continue;
				blockBallCollisionResult = CheckCollisionBetweenRects(block, state->balls[i], minCollisionTime);
				if (blockBallCollisionResult.collisions[1].side != gentle::None)
				{
					blockHitIndex = j;
					minCollisionTime = blockBallCollisionResult.time;
					hitBlockResult = blockBallCollisionResult;
					checkCollision = true;
				}
			}

			// Check for collision between ball and bat
			gentle::Rect<float> player = state->player;

			gentle::CollisionResult ballBatCollisionResult = gentle::CheckCollisionBetweenRects(player, state->balls[i], minCollisionTime);

			// Check collision results in the reverse order in which they are calculated
			// (player, block, wall)
			Ball newBallState;
			newBallState.velocity = state->balls[i].velocity;
			newBallState.halfSize = state->balls[i].halfSize;
			newBallState.exists = true;
			if (ballBatCollisionResult.collisions[1].side != gentle::None)
			{
				newBallState.position = ballBatCollisionResult.collisions[1].position;
				checkCollision = true;
				if (ballBatCollisionResult.collisions[1].side == gentle::Top)
				{
					// Add a horizontal velocity to allow player to change ball direction
					float ballAngleFromNormal = GetThetaForBallPlayerCollision(player.position.x, state->balls[i].position.x, player.halfSize.x);
					float ballSpeed = gentle::Length(state->balls[i].velocity);
					newBallState.velocity.x = (float)sin(ballAngleFromNormal) * ballSpeed;
					newBallState.velocity.y = (float)cos(ballAngleFromNormal) * ballSpeed;
				}
				else if (ballBatCollisionResult.collisions[1].side == gentle::Left)
				{
					float ballVelocityX = (state->balls[i].velocity.x > 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
					newBallState.velocity.x = MinFloat(ballVelocityX, player.velocity.x);
				}
				else
				{
					float ballVelocityX = (state->balls[i].velocity.x < 0) ? -state->balls[i].velocity.x : state->balls[i].velocity.x;
					newBallState.velocity.x = MaxFloat(ballVelocityX, player.velocity.x);
				}
			}
			else if (hitBlockResult.collisions[1].side != gentle::None)
			{
				newBallState.position = hitBlockResult.collisions[1].position;
				checkCollision = true;

				if (hitBlockResult.collisions[1].side == gentle::Top || hitBlockResult.collisions[1].side == gentle::Bottom)
				{
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (hitBlockResult.collisions[1].side == gentle::Top || hitBlockResult.collisions[1].side == gentle::Bottom)
				{
					newBallState.velocity.x = -newBallState.velocity.x;
				}

				Block *block = &state->blocks[blockHitIndex]; // Use derefence operator to update data in the blocks array here
				block->exists = false;
				state->score += BLOCK_SCORE;

				// Check for powerup
				if (block->powerUp.type != Nothing)
				{
					block->powerUp.exists = true;
				}
			}
			else if (ballWallCollision.result.collisions[0].side != gentle::None)
			{
				newBallState.position = ballWallCollision.result.collisions[0].position;
				checkCollision = true;

				if (ballWallCollision.wall.side == Left || ballWallCollision.wall.side == Right)
				{
					newBallState.velocity.x = -newBallState.velocity.x;
				}
				else if (ballWallCollision.wall.side == Top)
				{
					newBallState.velocity.y = -newBallState.velocity.y;
				}
				else if (ballWallCollision.wall.side == Bottom)
				{
					newBallState.exists = false;

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
			}
			else
			{
				newBallState.position = gentle::AddVectors(state->balls[i].position, gentle::MultiplyVectorByScalar(state->balls[i].velocity, t1 - t0));
				checkCollision = false;
			}

			state->balls[i] = newBallState;

			t0 = minCollisionTime;		// Update t0 so next ball position calculation starts from the collision time
			collisionCheckCount += 1;
		}

		float minBallSpeed = (allBlocksCleared) ? 0 : MIN_BALL_SPEED;
		// add some air resistance so ball slows down to normal speed after a while
		float ballSpeed = gentle::Length(state->balls[i].velocity);
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
			gentle::CollisionResult powerUpCollision = gentle::CheckCollisionBetweenRects(state->player, block->powerUp, dt);

			if (powerUpCollision.collisions[1].side != gentle::None)
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

	if (allBlocksCleared && (gentle::Length(state->balls[0].velocity) < LEVEL_CHANGE_BALL_SPEED))
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
	DrawRect(renderBuffer, GAME_RECT, BAT_COLOR, state.player.halfSize, state.player.position);

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

		DrawRect(renderBuffer, GAME_RECT, block->powerUp.color, block->powerUp.halfSize, block->powerUp.position);
	}

	// Balls, Level & Score
	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "BALLS", gentle::Vec2<float> { 10.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.lives, gentle::Vec2<float> { 25.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "LEVEL", gentle::Vec2<float> { 65.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.level, gentle::Vec2<float> { 80.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);

	DrawAlphabetCharacters(renderBuffer, GAME_RECT, "SCORE", gentle::Vec2<float> { 120.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
	DrawNumber(renderBuffer, GAME_RECT, state.score, gentle::Vec2<float> { 135.0f, 10.0f}, FONT_SIZE, TEXT_COLOR);
}

void gentle::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{

}

void gentle::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	gentle::Vec2<int> pixelRect;
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
