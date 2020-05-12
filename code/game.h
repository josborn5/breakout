#ifndef GAME_H
#define GAME_H

struct {
	Vector2D prevPosition;
	Vector2D position;
	Vector2D halfSize;
	Vector2D velocity;
} typedef Ball;

struct {
	Vector2D position;
	Vector2D halfSize;
	uint32_t color;
	b32 exists;
} typedef Block;

#endif
