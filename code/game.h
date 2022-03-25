#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3

#define ZERO_VECTOR gentle::Vec2<float> { 0.0f, 0.0f }

#include "../../win32-platform/bin/math.hpp"

typedef enum PowerUpType
{
	Nothing = 0,
	Multiball = 1,
	Comet = 2
} PowerUpType;

struct PowerUp
{
	gentle::Vec2<float> prevPosition;
	gentle::Vec2<float> position;
	gentle::Vec2<float> halfSize;
	gentle::Vec2<float> velocity;
	PowerUpType type;
	bool exists;		// flips to true when the block containing it is hit. i.e. it's a flag to render the power-up and include it in collision detection
	int color;
};

struct {
	gentle::Vec2<float> prevPosition;
	gentle::Vec2<float> position;
	gentle::Vec2<float> halfSize;
	gentle::Vec2<float> velocity;
} typedef Player;

struct {
	gentle::Vec2<float> prevPosition;
	gentle::Vec2<float> position;
	gentle::Vec2<float> halfSize;
	gentle::Vec2<float> velocity;
	bool exists;
} typedef Ball;

struct {
	gentle::Vec2<float> position;
	gentle::Vec2<float> halfSize;
	int color;
	bool exists;
	PowerUp powerUp;
} typedef Block;

struct {
	Player player;
	Ball balls[BALL_ARRAY_SIZE];
	Block blocks[BLOCK_ARRAY_SIZE];
	bool isCometActive;
	int level;
	int score;
	int lives;
} typedef GameState;

#endif
