#include <assert.h>

#include "game.c"

static void RunGetBallHorizontalVelocityFromTopPlayerCollisionTest(float playerPosX, float ballPosX, float playerHalfSizeX, float expectedResult)
{
	float result = GetBallHorizontalVelocityFromTopPlayerCollision(playerPosX, ballPosX, playerHalfSizeX);
	assert(result == expectedResult);
}

static void RunGameTests()
{
	RunGetBallHorizontalVelocityFromTopPlayerCollisionTest(20.0f, 15.0f, 5.0f, -20.0f);

	RunGetBallHorizontalVelocityFromTopPlayerCollisionTest(20.0f, 20.0f, 5.0f, 0.0f);

	RunGetBallHorizontalVelocityFromTopPlayerCollisionTest(20.0f, 25.0f, 5.0f, 20.0f);
}