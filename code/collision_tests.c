#include <stdio.h>
#include <assert.h>

#include "math.h"
#include "math.c"
#include "collision.c"

#define originalCollisionTime 10.0f
#define originPosition Vector2D { 0.0f, 0.0f }
#define oneByOneHalfSize Vector2D { 1.0f, 1.0f }

#define movingLeft Vector2D { -2.0f, 0.0f }
#define movingRight Vector2D { 2.0f, 0.0f }
#define movingUp Vector2D { 0.0f, 2.0f }
#define movingDown Vector2D { 0.0f, -2.0f }

#define blockRightOfOrigin Vector2D { 4.0f, 0.0f }
#define blockLeftOfOrigin Vector2D { -4.0f, 0.0f }
#define blockAboveOrigin Vector2D { 0.0f, 4.0f }
#define blockBelowOrigin Vector2D { 0.0f, -4.0f }

static void RunCheckCollisionBlockAndBallTests(Vector2D bPosition0, Vector2D bVelocity, float expectedCollisionTime, int expectedCollisionResult, Vector2D expectedCollisionPosition)
{
	Vector2D aHalfSize = oneByOneHalfSize;
	Vector2D aPosition0 = originPosition;

	Vector2D bHalfSize = oneByOneHalfSize;

	Vector2D bPosition1;
	bPosition1.x = bPosition0.x;
	bPosition1.y = bPosition0.y;
	float collisionTime = originalCollisionTime;
	int collisionResult = gentle::None;

	bool result = CheckBlockAndBallCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, bVelocity, &collisionTime, &collisionResult, &bPosition1);
	bool expectedCollision = (expectedCollisionResult != gentle::None);
	printf("collisionResult is %d\n", collisionResult);
	printf("collisionTime is %f\n", collisionTime);
	printf("collisionPosition.x is %f\n", bPosition1.x);
	printf("collisionPosition.y is %f\n\n", bPosition1.y);
	assert(result == expectedCollision);
	assert(collisionResult == expectedCollisionResult);
	assert(collisionTime == expectedCollisionTime);
	assert(bPosition1.x == expectedCollisionPosition.x);
	assert(bPosition1.y == expectedCollisionPosition.y);
}

static void RunCheckCollisionBetweenMovingObjectsTests(Vector2D aVelocity, Vector2D bPosition0, Vector2D bVelocity, float expectedCollisionTime, int expectedCollisionResult, Vector2D expectedCollisionPosition)
{
	Vector2D aHalfSize = oneByOneHalfSize;
	Vector2D aPosition0 = originPosition;

	Vector2D bHalfSize = oneByOneHalfSize;

	Vector2D bPosition1;
	bPosition1.x = bPosition0.x;
	bPosition1.y = bPosition0.y;
	float collisionTime = originalCollisionTime;
	int collisionResult = gentle::None;

	bool result = CheckCollisionBetweenMovingObjects(aHalfSize, aPosition0, aVelocity, bHalfSize, bPosition0, bVelocity, &collisionTime, &collisionResult, &bPosition1);
	bool expectedCollision = (expectedCollisionResult != gentle::None);
	printf("collisionResult is %d\n", collisionResult);
	printf("collisionTime is %f\n", collisionTime);
	printf("collisionPosition.x is %f\n", bPosition1.x);
	printf("collisionPosition.y is %f\n\n", bPosition1.y);
	assert(collisionTime == expectedCollisionTime);
	assert(collisionResult == expectedCollisionResult);
	assert(result == expectedCollision);
	assert(bPosition1.x == expectedCollisionPosition.x);
	assert(bPosition1.y == expectedCollisionPosition.y);
}

static void RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D blockPosition0, Vector2D blockVelocity, float expectedCollisionTime, int expectedCollisionResult, Vector2D expectedCollisionPosition)
{
	float wallYPos = 0.0f;
	Vector2D blockHalfSize = oneByOneHalfSize;

	Vector2D blockPosition1;
	blockPosition1.x = blockPosition0.x;
	blockPosition1.y = blockPosition0.y;
	float collisionTime = originalCollisionTime;
	int collisionResult = gentle::None;

	CheckBlockAndTopsideOfWallCollision(wallYPos, blockHalfSize, blockPosition0, blockVelocity, &collisionTime, &collisionResult, &blockPosition1);
	printf("collisionResult is %d\n", collisionResult);
	printf("collisionTime is %f\n", collisionTime);
	printf("collisionPosition.x is %f\n", blockPosition1.x);
	printf("collisionPosition.y is %f\n\n", blockPosition1.y);
	assert(collisionResult == expectedCollisionResult);
	assert(collisionTime == expectedCollisionTime);
	assert(blockPosition1.x == expectedCollisionPosition.x);
	assert(blockPosition1.y == expectedCollisionPosition.y);
}

static void RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D blockPosition0, Vector2D blockVelocity, float expectedCollisionTime, int expectedCollisionResult, Vector2D expectedCollisionPosition)
{
	float wallYPos = 0.0f;
	Vector2D blockHalfSize = oneByOneHalfSize;

	Vector2D blockPosition1;
	blockPosition1.x = blockPosition0.x;
	blockPosition1.y = blockPosition0.y;
	float collisionTime = originalCollisionTime;
	int collisionResult = gentle::None;

	CheckBlockAndUndersideOfWallCollision(wallYPos, blockHalfSize, blockPosition0, blockVelocity, &collisionTime, &collisionResult, &blockPosition1);
	printf("collisionResult is %d\n", collisionResult);
	printf("collisionTime is %f\n", collisionTime);
	printf("collisionPosition.x is %f\n", blockPosition1.x);
	printf("collisionPosition.y is %f\n\n", blockPosition1.y);
	assert(collisionResult == expectedCollisionResult);
	assert(collisionTime == expectedCollisionTime);
	assert(blockPosition1.x == expectedCollisionPosition.x);
	assert(blockPosition1.y == expectedCollisionPosition.y);
}

static void RunCollisionTests()
{
	/*
	 * CheckBlockAndBallCollision
	 *
	 */

	/* A <--B--> Right Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBlockAndBallTests(blockRightOfOrigin, movingLeft, 1, gentle::Right, Vector2D { 2.0f, 0.0f });

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests(blockRightOfOrigin, Vector2D { -0.20000001f, 0.0f }, originalCollisionTime, gentle::None, Vector2D { 4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests(blockRightOfOrigin, Vector2D { -0.20000002f, 0.0f }, 9.99999905f, gentle::Right, Vector2D { 2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	RunCheckCollisionBlockAndBallTests(blockRightOfOrigin, movingRight, originalCollisionTime, gentle::None, Vector2D { 4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { 4.0f, 2.0f }, movingLeft, 1, gentle::Right, Vector2D { 2.0f, 2.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { 4.0f, 2.000001f }, movingLeft, originalCollisionTime, gentle::None, Vector2D { 4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	RunCheckCollisionBlockAndBallTests(Vector2D  { 2.0f, 0.0f }, movingLeft, 0.0f, gentle::Right, Vector2D  { 2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	RunCheckCollisionBlockAndBallTests(Vector2D  { 2.0f, 0.0f }, movingRight, originalCollisionTime, gentle::None, Vector2D  { 2.0f, 0.0f });


	/* <--B--> A Left Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBlockAndBallTests(blockLeftOfOrigin, movingRight, 1, gentle::Left, Vector2D { -2.0f, 0.0f });

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests(blockLeftOfOrigin, Vector2D { 0.20000001f, 0.0f }, originalCollisionTime, gentle::None, Vector2D { -4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests(blockLeftOfOrigin, Vector2D { 0.20000002f, 0.0f }, 9.99999905f, gentle::Left, Vector2D { -2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	RunCheckCollisionBlockAndBallTests(blockLeftOfOrigin, movingLeft, originalCollisionTime, gentle::None, Vector2D { -4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(blockLeftOfOrigin, movingRight, 1, gentle::Left, Vector2D { -2.0f, 0.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { -4.0f, 2.000001f }, movingRight, originalCollisionTime, gentle::None, Vector2D { -4.0f, 2.000001f });

	// Collision when already touching and moving toward each other
	RunCheckCollisionBlockAndBallTests(Vector2D { -2.0f, 0.0f }, movingRight, 0.0f, gentle::Left, Vector2D { -2.0f, 0.0f });

	// No collision when already touching and moving away from each other
	RunCheckCollisionBlockAndBallTests(Vector2D  { -2.0f, 0.0f }, movingLeft, originalCollisionTime, gentle::None, Vector2D  { -2.0f, 0.0f });

	/* A	Bottom Side collisions
	 *
	 * Λ
	 * |
	 * B
	 * |
	 * V
	 */
	// collision on y-axis
	RunCheckCollisionBlockAndBallTests(blockBelowOrigin, movingUp, 1, gentle::Bottom, Vector2D { 0.0f, -2.0f });

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests(blockBelowOrigin, Vector2D { 0.0f, 0.20000001f }, originalCollisionTime, gentle::None, Vector2D { 0.0f, -4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests(blockBelowOrigin, Vector2D { 0.0f, 0.20000002f }, 9.99999905f, gentle::Bottom, Vector2D { 0.0f, -2.0f });

	// No collision on y axis when moving away from each other
	RunCheckCollisionBlockAndBallTests(blockBelowOrigin, movingDown, originalCollisionTime, gentle::None, Vector2D { 0.0f, -4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(blockBelowOrigin, movingUp, 1, gentle::Bottom, Vector2D { 0.0f, -2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { 2.000001f, -4.0f }, movingUp, originalCollisionTime, gentle::None, Vector2D { 2.000001f, -4.0f });

	// Collision when already touching and moving toward each other
	RunCheckCollisionBlockAndBallTests(Vector2D { 0.0f, -2.0f }, movingUp, 0.0f, gentle::Bottom, Vector2D { 0.0f, -2.0f });

	// No collision when already touching and moving away from each other
	RunCheckCollisionBlockAndBallTests(Vector2D  { 0.0f, -2.0f }, movingDown, originalCollisionTime, gentle::None, Vector2D  { 0.0f, -2.0f });

	/* Λ	Top Side collisions
	 * |
	 * B
	 * |
	 * V
	 *
	 * A
	 */
	// collision on y-axis
	RunCheckCollisionBlockAndBallTests(blockAboveOrigin, movingDown, 1, gentle::Top, Vector2D { 0.0f, 2.0f });

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests(blockAboveOrigin, Vector2D { 0.0f, -0.20000001f }, originalCollisionTime, gentle::None, Vector2D { 0.0f, 4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests(blockAboveOrigin, Vector2D { 0.0f, -0.20000002f }, 9.99999905f, gentle::Top, Vector2D { 0.0f, 2.0f });

	// No collision on y axis when moving away from each other
	RunCheckCollisionBlockAndBallTests(blockAboveOrigin, movingUp, originalCollisionTime, gentle::None, Vector2D { 0.0f, 4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { 2.0f, 4.0f }, movingDown, 1, gentle::Top, Vector2D { 2.0f, 2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests(Vector2D { 2.000001f, 4.0f }, movingDown, originalCollisionTime, gentle::None, Vector2D { 2.000001f, 4.0f });

	// Collision when already touching and moving toward each other
	RunCheckCollisionBlockAndBallTests(Vector2D { 0.0f, 2.0f }, movingDown, 0.0f, gentle::Top, Vector2D { 0.0f, 2.0f });

	// No collision when already touching and moving away from each other
	RunCheckCollisionBlockAndBallTests(Vector2D  { 0.0f, 2.0f }, movingUp, originalCollisionTime, gentle::None, Vector2D  { 0.0f, 2.0f });

	/*
	 * CheckCollisionBetweenMovingObjects
	 *
	 */

	// collision on x-axis: A--> <--B Right Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests(movingRight, blockRightOfOrigin, movingLeft, 0.5f, gentle::Right, Vector2D { 3.0f, 0.0f });

	// collision on x-axis: <-A <--B Right Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests(Vector2D { -1.0f, 0.0f }, blockRightOfOrigin, movingLeft, 2.0f, gentle::Right, Vector2D { 0.0f, 0.0f });

	// collision on x-axis: B--> <--A Left Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests(movingLeft, blockLeftOfOrigin, movingRight, 0.5f, gentle::Left, Vector2D { -3.0f, 0.0f });

	// collision on x-axis: B--> A-> Left Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests(Vector2D { 1.0f, 0.0f }, blockLeftOfOrigin, movingRight, 2.0f, gentle::Left, Vector2D { 0.0f, 0.0f });

	/* A	Bottom Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * B
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests(movingDown, blockBelowOrigin, movingUp, 0.5f, gentle::Bottom, Vector2D { 0.0f, -3.0f });

	/* Λ	Bottom Side collisions
	 * |
	 * A
	 *
	 * Λ
	 * |
	 * |
	 * B
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests(Vector2D { 0.0f, 1.0f }, blockBelowOrigin, movingUp, 2.0f, gentle::Bottom, Vector2D { 0.0f, 0.0f });

	/* B	Top Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * A
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests(movingUp, blockAboveOrigin, movingDown, 0.5f, gentle::Top, Vector2D { 0.0f, 3.0f });

	/* B	Top Side collisions
	 * |
	 * |
	 * V
	 *
	 * A
	 * |
	 * V
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests(Vector2D { 0.0f, -1.0f }, blockAboveOrigin, movingDown, 2.0f, gentle::Top, Vector2D { 0.0f, 0.0f });


	/*
	 * CheckBlockAndTopsideOfWallCollision
	 *
	 */

	/* B
	 * |
	 * V
	 *---
	 */
	RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D { 10.0f, 4.0f }, movingDown, 1.5f, gentle::Top, Vector2D { 10.0f, 1.0f });

	/* B
	 *---
	 * |
	 * V
	 */
	RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D { -10.0f, 1.0f }, movingDown, 0.0f, gentle::Top, Vector2D { -10.0f, 1.0f });

	/* Λ
	 * |
	 * B
	 *---
	 */
	RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D { 0.0f, 1.0f }, movingUp, originalCollisionTime, gentle::None, Vector2D { 0.0f, 1.0f });

	/* B-->
	 *------
	 */
	RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D { 0.0f, 1.0f }, movingRight, originalCollisionTime, gentle::None, Vector2D { 0.0f, 1.0f });

	/* <--B
	 *------
	 */
	RunCheckBlockAndTopsideOfWallCollisionTest(Vector2D { 0.0f, 1.0f }, movingLeft, originalCollisionTime, gentle::None, Vector2D { 0.0f, 1.0f });


	/*
	 * CheckBlockAndUndersideOfWallCollision
	 *
	 */

	 /*---
	  * Λ
	  * |
	  * B
	  */
	RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D { 10.0f, -4.0f }, movingUp, 1.5f, gentle::Bottom, Vector2D { 10.0f, -1.0f });

	/* Λ
	 * |
	 *---
	 * B
	 */
	RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D { -10.0f, -1.0f }, movingUp, 0.0f, gentle::Bottom, Vector2D { -10.0f, -1.0f });

	/*---
	 * B
	 * |
	 * V
	 */
	RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D { 0.0f, -1.0f }, movingDown, originalCollisionTime, gentle::None, Vector2D { 0.0f, -1.0f });

	/*------
	 *B-->
	 */
	RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D { 0.0f, -1.0f }, movingRight, originalCollisionTime, gentle::None, Vector2D { 0.0f, -1.0f });

	/*------
	 * <--B
	 */
	RunCheckBlockAndUndersideOfWallCollisionTest(Vector2D { 0.0f, -1.0f }, movingLeft, originalCollisionTime, gentle::None, Vector2D { 0.0f, -1.0f });
}