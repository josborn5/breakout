 internal b32 AABBCollideRectToRect(Vector2D aHalfSize, Vector2D aPos, Vector2D bHalfSize, Vector2D bPos)
{
	Vector2D bBottomLeft = SubtractVector2D(bPos, bHalfSize);	// Bottom left co-ords of b
	Vector2D bTopRight = AddVector2D(bPos, bHalfSize);			// Top rght co-ords of b
	Vector2D aBottomLeft = SubtractVector2D(aPos, aHalfSize);	// Bottom left co-ords of a
	Vector2D aTopRight = AddVector2D(aPos, aHalfSize);			// Top right co-ords of a

	return AABBCollideCornerToCorner(aTopRight, aBottomLeft, bTopRight, bBottomLeft);
}

 internal b32 AABBCollideCornerToRect(Vector2D aHalfSize, Vector2D aPos, Vector2D bTopRight, Vector2D bBottomLeft)
{
	Vector2D aBottomLeft = SubtractVector2D(aPos, aHalfSize);	// Bottom left co-ords of a
	Vector2D aTopRight = AddVector2D(aPos, aHalfSize);			// Top right co-ords of a

	return AABBCollideCornerToCorner(aTopRight, aBottomLeft, bTopRight, bBottomLeft);
}

internal b32 AABBCollideCornerToCorner(Vector2D aTopRight, Vector2D aBottomLeft, Vector2D bTopRight, Vector2D bBottomLeft)
{
	b32 verticalCollision = (bBottomLeft.y < aTopRight.y && bTopRight.y > aBottomLeft.y);
	b32 horizontalCollision = (bBottomLeft.x < aTopRight.x && bTopRight.x > aBottomLeft.x);
	return verticalCollision && horizontalCollision;
}

internal b32 AABBCollideRectToVertical(Vector2D aHalfSize, Vector2D aPos, float horizontalPos)
{
	return ((aPos.x + aHalfSize.x) > horizontalPos && (aPos.x - aHalfSize.x) < horizontalPos);
}

internal b32 AABBCollideRectToHorizontal(Vector2D aHalfSize, Vector2D aPos, float verticalPos)
{
	return ((aPos.y + aHalfSize.y) > verticalPos && (aPos.y - aHalfSize.y) < verticalPos);
}

typedef enum BlockSide
{
	Top = 0,
	Right = 1,
	Bottom = 2,
	Left = 3,
	None = 4
} BlockSide;

internal b32 CheckBlockAndBallCollision(Vector2D blockHalfSize, Vector2D blockP, Vector2D ballHalfSize, Vector2D prevBallPosition, Vector2D ballVelocity, float *maxCollisionTime, int *blockResult)
{
	b32 collided = false;
	float blockTopSide = blockP.y + blockHalfSize.y + ballHalfSize.y;
	float blockBottomSide = blockP.y - blockHalfSize.y - ballHalfSize.y;
	float blockLeftSide = blockP.x - blockHalfSize.x - ballHalfSize.x;
	float blockRightSide = blockP.x + blockHalfSize.x + ballHalfSize.x;

	// Check for collision between block side and ball path
	// 1. Top/bottom side
	float tHorizontalCollision = -1.0f;
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
			}
		}
	}

	// 2. Left/right side
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
			}
		}
	}
	return collided;
}