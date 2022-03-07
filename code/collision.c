#include "utils.h"
#include "math.h"

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
	if (tCollision >= 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Bottom;
		float xCollisionPos = prevBallPosition.x + (tCollision * ballVelocity.x);
		ballPosition->x = xCollisionPos;
		ballPosition->y = yCollisionCheckPos;
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
	if (tCollision >= 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Top;
		float xCollisionPos = prevBallPosition.x + (tCollision * ballVelocity.x);
		ballPosition->x = xCollisionPos;
		ballPosition->y = yCollisionCheckPos;
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
	if (tCollision >= 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Right;
		float yCollisionPos = prevBallPosition.y + (tCollision * ballVelocity.y);
		ballPosition->x = xCollisionCheckPos;
		ballPosition->y = yCollisionPos;
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
	if (tCollision >= 0 && tCollision < *maxCollisionTime)
	{
		*maxCollisionTime = tCollision;
		*blockResult = Left;
		float yCollisionPos = prevBallPosition.y + (tCollision * ballVelocity.y);
		ballPosition->x = xCollisionCheckPos;
		ballPosition->y = yCollisionPos;
	}
}

static bool CheckBlockAndBallCollision(
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
	bool collided = false;
	float blockTopSide = blockP.y + blockHalfSize.y + ballHalfSize.y;
	float blockBottomSide = blockP.y - blockHalfSize.y - ballHalfSize.y;
	float blockLeftSide = blockP.x - blockHalfSize.x - ballHalfSize.x;
	float blockRightSide = blockP.x + blockHalfSize.x + ballHalfSize.x;

	// Check for collision between block side and ball path
	// 1. Top/bottom side
	int horizontalCollisionResult = None;
	float yCollisionCheckPos = 0.0f;
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
		if (tYCollision >= 0)
		{
			float ballXPosAtCollision = prevBallPosition.x + (tYCollision * ballVelocity.x);
			if (ballXPosAtCollision >= blockLeftSide && ballXPosAtCollision <= blockRightSide && tYCollision < *maxCollisionTime)
			{
				*maxCollisionTime = tYCollision;
				*blockResult = horizontalCollisionResult;
				collided = true;
				ballPosition->x = ballXPosAtCollision;
				ballPosition->y = yCollisionCheckPos;
			}
		}
	}

	// 2. Left/right side, 
	int verticalCollisionResult = None;
	float xCollisionCheckPos = 0.0f;
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
		if (tXCollision >= 0)
		{
			float ballYPosAtCollision = prevBallPosition.y + (tXCollision * ballVelocity.y);
			if (ballYPosAtCollision >= blockBottomSide && ballYPosAtCollision <= blockTopSide && tXCollision < *maxCollisionTime)
			{
				*maxCollisionTime = tXCollision;
				*blockResult = verticalCollisionResult;
				collided = true;
				ballPosition->x = xCollisionCheckPos;
				ballPosition->y = ballYPosAtCollision;
			}
		}
	}
	return collided;
}

static bool CheckCollisionBetweenMovingObjects(
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

	bool result = CheckBlockAndBallCollision(aHalfSize, aPosition0, bHalfSize, bPosition0, aRelBVelocity, maxCollisionTime, collisionResult, bPosition1);

	// Translate bPosition1 from the co-ordinate system whose origin is on 'a' back to the static co-ordinate system
	if (result)
	{
		Vector2D deltaAPosition = MultiplyVector2D(aVelocity, *maxCollisionTime);
		*bPosition1 = AddVector2D(deltaAPosition, *bPosition1);
	}
	return result;
}