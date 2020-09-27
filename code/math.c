#include <stdint.h>
#include <math.h>
#include "utils.h"
#include "math.h"

static float MaxFloat(float val1, float val2)
{
	if (val1 > val2)
	{
		return val1;
	}
	else
	{
		return val2;
	}
}

static float MinFloat(float val1, float val2)
{
	if (val1 < val2)
	{
		return val1;
	}
	else
	{
		return val2;
	}
}

static int ClampInt(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static float ClampFloat(float min, float val, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static uint32_t MakeColorFromGrey(uint8_t grey)
{
	return (grey << 0) | (grey << 8) | (grey << 16);
}

static Vector2D AddVector2D(Vector2D a, Vector2D b)
{
	Vector2D v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	return v;
}

static Vector2D SubtractVector2D(Vector2D a, Vector2D b)
{
	Vector2D v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	return v;
}

static Vector2D MultiplyVector2D(Vector2D a, float scalar)
{
	Vector2D v;
	v.x = scalar * a.x;
	v.y = scalar * a.y;
	return v;
}

static float GetVectorMagnitude(Vector2D vector)
{
	return (float)sqrt((vector.x * vector.x) + (vector.y * vector.y));
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
