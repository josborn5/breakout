#include <stdio.h>
#include <assert.h>

#include "math.h"
#include "math.c"
#include "collision.c"

#define originalCollisionTime 10.0f

static void RunCheckCollisionBlockAndBallTests(Vector2D bPosition0, Vector2D bVelocity, float expectedCollisionTime, int expectedCollisionResult, Vector2D expectedCollisionPosition)
{
	Vector2D aHalfSize = (Vector2D){ 1.0f, 1.0f };
	Vector2D aPosition0 = (Vector2D){ 0.0f, 0.0f };

	Vector2D bHalfSize = (Vector2D){ 1.0f, 1.0f };

	Vector2D bPosition1 = (Vector2D){ bPosition0.x, bPosition0.y };
	float collisionTime = originalCollisionTime;
	int collisionResult = None;

	b32 result = CheckBlockAndBallCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, bVelocity, &collisionTime, &collisionResult, &bPosition1);
	b32 expectedCollision = (expectedCollisionResult != None);
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
	Vector2D aHalfSize = (Vector2D){ 1.0f, 1.0f };
	Vector2D aPosition0 = (Vector2D){ 0.0f, 0.0f };

	Vector2D bHalfSize = (Vector2D){ 1.0f, 1.0f };

	Vector2D bPosition1 = (Vector2D){ bPosition0.x, bPosition0.y };
	float collisionTime = originalCollisionTime;
	int collisionResult = None;

	b32 result = CheckCollisionBetweenMovingObjects(aHalfSize, aPosition0, aVelocity, bHalfSize, bPosition0, bVelocity, &collisionTime, &collisionResult, &bPosition1);
	b32 expectedCollision = (expectedCollisionResult != None);
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

static void RunCollisionTests()
{
	/*
	 * CheckBlockAndBallCollision
	 *
	 */

	/* A <--B--> Right Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, 1, Right, (Vector2D){ 2.0f, 0.0f });

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -0.20000001f, 0.0f }, originalCollisionTime, None, (Vector2D){ 4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 0.0f }, (Vector2D){ -0.20000002f, 0.0f }, 9.99999905f, Right, (Vector2D){ 2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, originalCollisionTime, None, (Vector2D){ 4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 2.0f }, (Vector2D){ -2.0f, 0.0f }, 1, Right, (Vector2D){ 2.0f, 2.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 4.0f, 2.000001f }, (Vector2D){ -2.0f, 0.0f }, originalCollisionTime, None, (Vector2D){ 4.0f, 2.000001f });

	/* <--B--> A Left Hand Side collisions */
	// collision on x-axis
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, 1, Left, (Vector2D){ -2.0f, 0.0f });

	// No collision on x-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 0.20000001f, 0.0f }, originalCollisionTime, None, (Vector2D){ -4.0f, 0.0f });

	// Collision on x-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 0.0f }, (Vector2D){ 0.20000002f, 0.0f }, 9.99999905f, Left, (Vector2D){ -2.0f, 0.0f });

	// No collision on x axis when moving away from each other
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, originalCollisionTime, None, (Vector2D){ -4.0f, 0.0f });

	// Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 2.0f }, (Vector2D){ 2.0f, 0.0f }, 1, Left, (Vector2D){ -2.0f, 2.0f });

	// No Collision on x axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ -4.0f, 2.000001f }, (Vector2D){ 2.0f, 0.0f }, originalCollisionTime, None, (Vector2D){ -4.0f, 2.000001f });

	/* A	Bottom Side collisions
	 *
	 * Λ
	 * |
	 * B
	 * |
	 * V
	 */
	// collision on y-axis
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 1, Bottom, (Vector2D){ 0.0f, -2.0f });

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 0.20000001f }, originalCollisionTime, None, (Vector2D){ 0.0f, -4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 0.20000002f }, 9.99999905f, Bottom, (Vector2D){ 0.0f, -2.0f });

	// No collision on y axis when moving away from each other
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, -2.0f }, originalCollisionTime, None, (Vector2D){ 0.0f, -4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 2.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 1, Bottom, (Vector2D){ 2.0f, -2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 2.000001f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, originalCollisionTime, None, (Vector2D){ 2.000001f, -4.0f });

	/* Λ	Top Side collisions
	 * |
	 * B
	 * |
	 * V
	 *
	 * A
	 */
	// collision on y-axis
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 1, Top, (Vector2D){ 0.0f, 2.0f });

	// No collision on y-axis when moving too slow to collide before collision time boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -0.20000001f }, originalCollisionTime, None, (Vector2D){ 0.0f, 4.0f });

	// Collision on y-axis when moving slow to collide on collision time
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -0.20000002f }, 9.99999905f, Top, (Vector2D){ 0.0f, 2.0f });

	// No collision on y axis when moving away from each other
	RunCheckCollisionBlockAndBallTests((Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, 2.0f }, originalCollisionTime, None, (Vector2D){ 0.0f, 4.0f });

	// Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 2.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 1, Top, (Vector2D){ 2.0f, 2.0f });

	// No Collision on y axis when moving toward each other off center at boundary
	RunCheckCollisionBlockAndBallTests((Vector2D){ 2.000001f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, originalCollisionTime, None, (Vector2D){ 2.000001f, 4.0f });

	/*
	 * CheckCollisionBetweenMovingObjects
	 *
	 */

	// collision on x-axis: A--> <--B Right Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 2.0f, 0.0f }, (Vector2D){ 4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, 0.5f, Right, (Vector2D){ 3.0f, 0.0f });

	// collision on x-axis: <-A <--B Right Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ -1.0f, 0.0f }, (Vector2D){ 4.0f, 0.0f }, (Vector2D){ -2.0f, 0.0f }, 2.0f, Right, (Vector2D){ 0.0f, 0.0f });

	// collision on x-axis: B--> <--A Left Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ -2.0f, 0.0f }, (Vector2D){ -4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, 0.5f, Left, (Vector2D){ -3.0f, 0.0f });

	// collision on x-axis: B--> A-> Left Hand Side collisions
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 1.0f, 0.0f }, (Vector2D){ -4.0f, 0.0f }, (Vector2D){ 2.0f, 0.0f }, 2.0f, Left, (Vector2D){ 0.0f, 0.0f });

	/* A	Bottom Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * B
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 0.0f, -2.0f }, (Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 0.5f, Bottom, (Vector2D){ 0.0f, -3.0f });

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
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 0.0f, 1.0f }, (Vector2D){ 0.0f, -4.0f }, (Vector2D){ 0.0f, 2.0f }, 2.0f, Bottom, (Vector2D){ 0.0f, 0.0f });

	/* B	Top Side collisions
	 * |
	 * V
	 *
	 * Λ
	 * |
	 * A
	 */
	 // collision on y-axis
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 0.0f, 2.0f }, (Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 0.5f, Top, (Vector2D){ 0.0f, 3.0f });

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
	RunCheckCollisionBetweenMovingObjectsTests((Vector2D){ 0.0f, -1.0f }, (Vector2D){ 0.0f, 4.0f }, (Vector2D){ 0.0f, -2.0f }, 2.0f, Top, (Vector2D){ 0.0f, 0.0f });
}