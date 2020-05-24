#ifndef GAME_H
#define GAME_H

#define BLOCK_ARRAY_SIZE 64
#define BALL_ARRAY_SIZE 3
#define POWER_UPS_IN_EFFECT_SIZE 2
#define POWER_UPS_SIZE 8

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
	b32 exists;
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
	b32 exists;
} typedef Ball;

struct {
	Vector2D position;
	Vector2D halfSize;
	uint32_t color;
	b32 exists;
	PowerUpType powerUpType;
} typedef Block;

struct {
	Player player;
	Ball balls[BALL_ARRAY_SIZE];
	Block blocks[BLOCK_ARRAY_SIZE];
	PowerUp powerUps[POWER_UPS_SIZE];
	PowerUpType powerUpsInEffect[POWER_UPS_IN_EFFECT_SIZE];
	int level;
	int score;
	int lives;
} typedef GameState;

#endif
