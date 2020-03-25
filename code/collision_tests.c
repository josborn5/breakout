#include <stdio.h>
#include <assert.h>

#include "math.h"
#include "math.c"
#include "collision.c"

static originalCollisionTime = 10.0f;

static void RunCheckCollisionBetweenMovingObjectsTest(Vector2D bPosition0, Vector2D bVelocity, float expectedCollisionTime, int expectedCollisionResult)
{
	Vector2D aHalfSize = (Vector2D){ 1.0f, 1.0f};
	Vector2D aPosition0 = (Vector2D){ 0.0f, 0.0f};

	Vector2D bHalfSize = (Vector2D){ 1.0f, 1.0f};
	// Vector2D bPosition0 = (Vector2D){ 4.0f, 0.0f};
	// Vector2D bVelocity = (Vector2D){ -2.0f, 0.0f};

	Vector2D bPosition1 = (Vector2D){ 0.0f, 0.0f};
	float collisionTime = originalCollisionTime;
	int collisionResult = None;

	b32 result = CheckBlockAndBallCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, bVelocity, &collisionTime, &collisionResult, &bPosition1);
	printf("collisionTime is %f\n", collisionTime);
	printf("collisionResult is %d\n\n", collisionResult);
	assert(collisionTime == expectedCollisionTime);
	assert(collisionResult == expectedCollisionResult);
}

static void RunCollisionTests()
{
	/*
	 * CheckBlockAndBallCollision
	 *
	 */

	// collision on x-axis
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f}, (Vector2D){ -2.0f, 0.0f}, 1, Right);

	// No collision on x axis when moving away from each other
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f}, (Vector2D){ 2.0f, 0.0f}, originalCollisionTime, None);

	// Collision on x axis when moving toward each other off center
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 1.5f}, (Vector2D){ -2.0f, 0.0f}, 1, Right);
}