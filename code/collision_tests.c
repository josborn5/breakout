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

	/* A <--B--> Right Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, 1, Right);

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -0.20000001f, 0.0f }, originalCollisionTime, None);

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -0.20000002f, 0.0f }, 9.99999905f, Right);

	// No collision on x axis when moving away from each other
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, originalCollisionTime, None);

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 2.0f }, (Vector2D){ -2.0f, 0.0f }, 1, Right);

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 4.0f, 2.000001f }, (Vector2D){ -2.0f, 0.0f }, originalCollisionTime, None);

	/* <--B--> A Left Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, 1, Left);

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 0.20000001f, 0.0f }, originalCollisionTime, None);

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 0.20000002f, 0.0f }, 9.99999905f, Left);

	// No collision on x axis when moving away from each other
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, originalCollisionTime, None);

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 2.0f }, (Vector2D){ 2.0f, 0.0f }, 1, Left);

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ -4.0f, 2.000001f }, (Vector2D){ 2.0f, 0.0f }, originalCollisionTime, None);

	/* A	Bottom Side collisions
	 *
	 * Λ
	 * |
	 * B
	 * |
	 * V
	 */
	// collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 1, Bottom);

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 0.20000001f }, originalCollisionTime, None);

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 0.20000002f }, 9.99999905f, Bottom);

	// No collision on y axis when moving away from each other
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, -2.0f }, originalCollisionTime, None);

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 2.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 1, Bottom);

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 2.000001f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, originalCollisionTime, None);

	/* Λ	Top Side collisions
	 * |
	 * B
	 * |
	 * V
	 *
	 * A
	 */
	// collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 1, Top);

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -0.20000001f }, originalCollisionTime, None);

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -0.20000002f }, 9.99999905f, Top);

	// No collision on y axis when moving away from each other
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, 2.0f }, originalCollisionTime, None);

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 2.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 1, Top);

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBetweenMovingObjectsTest((Vector2D){ 2.000001f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, originalCollisionTime, None);
}