#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3

#define ZERO_VECTOR Vector2D { 0.0f, 0.0f }

#define POWER_UP_HALF_SIZE Vector2D { 0.5f, 0.5f }
#define POWER_UP_VELOCITY Vector2D { 0.0f, -5.0f }

#include <stdint.h>

typedef enum PowerUpType
{
	Nothing = 0,
	Multiball = 1,
	Comet = 2
} PowerUpType;

struct {
	Vector2D prevPosition;
	Vector2D position;
	Vector2D halfSize;
	Vector2D velocity;
	PowerUpType type;
	bool exists;		// flips to true when the block containing it is hit. i.e. it's a flag to render the power-up and include it in collision detection
	uint32_t color;
} typedef PowerUp;

struct {
	Vector2D prevPosition;
	Vector2D position;
	Vector2D halfSize;
	Vector2D velocity;
} typedef Player;

struct {
	Vector2D prevPosition;
	Vector2D position;
	Vector2D halfSize;
	Vector2D velocity;
	bool exists;
} typedef Ball;

struct {
	Vector2D position;
	Vector2D halfSize;
	uint32_t color;
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
