#include "utils.h"
#include "math.h"

static b32 AABBCollideRectToRect(Vector2D aHalfSize, Vector2D aPos, Vector2D bHalfSize, Vector2D bPos)
{
	Vector2D bBottomLeft = SubtractVector2D(bPos, bHalfSize);	// Bottom left co-ords of b
	Vector2D bTopRight = AddVector2D(bPos, bHalfSize);			// Top rght co-ords of b
	Vector2D aBottomLeft = SubtractVector2D(aPos, aHalfSize);	// Bottom left co-ords of a
	Vector2D aTopRight = AddVector2D(aPos, aHalfSize);			// Top right co-ords of a

	return AABBCollideCornerToCorner(aTopRight, aBottomLeft, bTopRight, bBottomLeft);
}

static b32 AABBCollideCornerToRect(Vector2D aHalfSize, Vector2D aPos, Vector2D bTopRight, Vector2D bBottomLeft)
{
	Vector2D aBottomLeft = SubtractVector2D(aPos, aHalfSize);	// Bottom left co-ords of a
	Vector2D aTopRight = AddVector2D(aPos, aHalfSize);			// Top right co-ords of a

	return AABBCollideCornerToCorner(aTopRight, aBottomLeft, bTopRight, bBottomLeft);
}

static b32 AABBCollideCornerToCorner(Vector2D aTopRight, Vector2D aBottomLeft, Vector2D bTopRight, Vector2D bBottomLeft)
{
	b32 verticalCollision = (bBottomLeft.y < aTopRight.y && bTopRight.y > aBottomLeft.y);
	b32 horizontalCollision = (bBottomLeft.x < aTopRight.x && bTopRight.x > aBottomLeft.x);
	return verticalCollision && horizontalCollision;
}

static b32 AABBCollideRectToVertical(Vector2D aHalfSize, Vector2D aPos, float horizontalPos)
{
	return ((aPos.x + aHalfSize.x) > horizontalPos && (aPos.x - aHalfSize.x) < horizontalPos);
}

typedef enum BlockSide
{
	Top = 0,
	Right = 1,
	Bottom = 2,
	Left = 3,
	None = 4
} BlockSide;

static void CheckBlockAndUndersideOfWallCollision(
	float wallYPos,
	Vector2D ballHalfSize,
	Vector2D prevBallPosition,
	Vector2D ballVelocity,
	float *maxCollisionTime,
	int *blockResult,
	Vector2D *ballPosition
)
{
	if (ballVelocity.y <= 0) return;

	// Check for collision between ball and underside of wall
	float yCollisionCheckPos = wallYPos - ballHalfSize.y;

	float tCollision = (yCollisionCheckPos - prevBallPosition.y) / ballVelocity.y;
	if (tCollision > 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Bottom;
		float xCollisionPos = prevBallPosition.x + (tCollision * ballVelocity.x);
		*ballPosition = (Vector2D) {xCollisionPos, yCollisionCheckPos};
	}
}

static void CheckBlockAndTopsideOfWallCollision(
	float wallYPos,
	Vector2D ballHalfSize,
	Vector2D prevBallPosition,
	Vector2D ballVelocity,
	float *maxCollisionTime,
	int *blockResult,
	Vector2D *ballPosition
)
{
	if (ballVelocity.y >= 0) return;

	// Check for collision between ball and topside of wall
	float yCollisionCheckPos = wallYPos + ballHalfSize.y;

	float tCollision = (yCollisionCheckPos - prevBallPosition.y) / ballVelocity.y;
	if (tCollision > 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Top;
		float xCollisionPos = prevBallPosition.x + (tCollision * ballVelocity.x);
		*ballPosition = (Vector2D) {xCollisionPos, yCollisionCheckPos};
	}
}

static void CheckBlockAndLeftWallCollision(
	float wallXPos,
	Vector2D ballHalfSize,
	Vector2D prevBallPosition,
	Vector2D ballVelocity,
	float *maxCollisionTime,
	int *blockResult,
	Vector2D *ballPosition
)
{
	if (ballVelocity.x >= 0) return;

	// Check for collision between ball and left wall
	float xCollisionCheckPos = wallXPos + ballHalfSize.x;

	float tCollision = (xCollisionCheckPos - prevBallPosition.x) / ballVelocity.x;
	if (tCollision > 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Right;
		float yCollisionPos = prevBallPosition.y + (tCollision * ballVelocity.y);
		*ballPosition = (Vector2D) { xCollisionCheckPos, yCollisionPos };
	}
}

static void CheckBlockAndRightWallCollision(
	float wallXPos,
	Vector2D ballHalfSize,
	Vector2D prevBallPosition,
	Vector2D ballVelocity,
	float *maxCollisionTime,
	int *blockResult,
	Vector2D *ballPosition
)
{
	if (ballVelocity.x <= 0) return;

	// Check for collision between ball and right wall
	float xCollisionCheckPos = wallXPos - ballHalfSize.x;

	float tCollision = (xCollisionCheckPos - prevBallPosition.x) / ballVelocity.x;
	if (tCollision > 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Left;
		float yCollisionPos = prevBallPosition.y + (tCollision * ballVelocity.y);
		*ballPosition = (Vector2D) {xCollisionCheckPos, yCollisionPos};
	}
}

static b32 CheckBlockAndBallCollision(
	Vector2D blockHalfSize,
	Vector2D blockP,
	Vector2D ballHalfSize,
	Vector2D prevBallPosition,
	Vector2D ballVelocity,
	float *maxCollisionTime,
	int *blockResult,
	Vector2D *ballPosition
)
{
	b32 collided = false;
	float blockTopSide = blockP.y + blockHalfSize.y + ballHalfSize.y;
	float blockBottomSide = blockP.y - blockHalfSize.y - ballHalfSize.y;
	float blockLeftSide = blockP.x - blockHalfSize.x - ballHalfSize.x;
	float blockRightSide = blockP.x + blockHalfSize.x + ballHalfSize.x;

	// Check for collision between block side and ball path
	// 1. Top/bottom side
	int horizontalCollisionResult = None;
	float yCollisionCheckPos;
	if (ballVelocity.y > 0)
	{
		yCollisionCheckPos = blockBottomSide;
		horizontalCollisionResult = Bottom;
	}
	if (ballVelocity.y < 0)
	{
		yCollisionCheckPos = blockTopSide;
		horizontalCollisionResult = Top;
	}

	if (ballVelocity.y != 0)
	{
		float tYCollision = (yCollisionCheckPos - prevBallPosition.y) / ballVelocity.y;
		if (tYCollision > 0)
		{
			float ballXPosAtCollision = prevBallPosition.x + (tYCollision * ballVelocity.x);
			if (ballXPosAtCollision >= blockLeftSide && ballXPosAtCollision <= blockRightSide && tYCollision < *maxCollisionTime)
			{
				*maxCollisionTime = tYCollision;
				*blockResult = horizontalCollisionResult;
				collided = true;
				*ballPosition = (Vector2D) { ballXPosAtCollision, yCollisionCheckPos };
			}
		}
	}

	// 2. Left/right side, 
	float tVerticalCollision = -1.0f;
	int verticalCollisionResult = None;
	float xCollisionCheckPos;
	if (ballVelocity.x > 0)
	{
		xCollisionCheckPos = blockLeftSide;
		verticalCollisionResult = Left;
	}
	if (ballVelocity.x < 0)
	{
		xCollisionCheckPos = blockRightSide;
		verticalCollisionResult = Right;
	}

	if (ballVelocity.x != 0)
	{
		float tXCollision = (xCollisionCheckPos - prevBallPosition.x) / ballVelocity.x;
		if (tXCollision > 0)
		{
			float ballYPosAtCollision = prevBallPosition.y + (tXCollision * ballVelocity.y);
			if (ballYPosAtCollision >= blockBottomSide && ballYPosAtCollision <= blockTopSide && tXCollision < *maxCollisionTime)
			{
				*maxCollisionTime = tXCollision;
				*blockResult = verticalCollisionResult;
				collided = true;
				*ballPosition = (Vector2D) { xCollisionCheckPos, ballYPosAtCollision };
			}
		}
	}
	return collided;
}

static b32 CheckCollisionBetweenMovingObjects(
	Vector2D aHalfSize,
	Vector2D aPosition0,
	Vector2D aVelocity,
	Vector2D bHalfSize,
	Vector2D bPosition0,
	Vector2D bVelocity,
	float *maxCollisionTime,
	int *collisionResult,
	Vector2D *bPosition1
)
{
	// Calculate relative velocity as between a & b, as if a is static. i.e. the origin of our co-ordinate system is fixed to whereever object 'a' is
	Vector2D aRelBVelocity = SubtractVector2D(bVelocity, aVelocity);

	b32 result = CheckBlockAndBallCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, aRelBVelocity, maxCollisionTime, collisionResult, bPosition1);

	// Translate bPosition1 from the co-ordinate system whose origin is on 'a' back to the static co-ordinate system
	if (result)
	{
		Vector2D deltaAPosition = MultiplyVector2D(aVelocity, *maxCollisionTime);
		*bPosition1 = AddVector2D(deltaAPosition, *bPosition1);
	}
	return result;
}