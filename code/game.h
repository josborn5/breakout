#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3

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
	b32 exists;
} typedef Ball;

struct {
	Vector2D position;
	Vector2D halfSize;
	uint32_t color;
	b32 exists;
} typedef Block;

struct {
	Player player;
	Ball balls[BALL_ARRAY_SIZE];
	Block blocks[BLOCK_ARRAY_SIZE];
	int level;
	int score;
	int lives;
} typedef GameState;

#endif
