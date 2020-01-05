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

internal int CheckBlockAndBallCollision(Vector2D blockHalfSize, Vector2D blockP, Vector2D ballHalfSize, Vector2D ballP0, Vector2D ballP1, Vector2D ballVelocity)
{
	int collisionResult = None;
	b32 isCollision = AABBCollideRectToRect(ballHalfSize, ballP1, blockHalfSize, blockP);
	if (isCollision)
	{
		if (CollisionOnBrickBottom(blockHalfSize, blockP, ballHalfSize, ballP0, ballP1))
		{
			collisionResult = Bottom;
		}
		else if (CollisionOnBrickTop(blockHalfSize, blockP, ballHalfSize, ballP0, ballP1))
		{
			collisionResult = Top;
		}
		else
		{
			collisionResult = Left;
		}
	}
	
	return collisionResult;
}

internal b32 CollisionOnBrickBottom(Vector2D blockHalfSize, Vector2D blockPosition, Vector2D ballHalfSize, Vector2D ballP0, Vector2D ballP1)
{
	Vector2D blockP0 = SubtractVector2D(blockPosition, blockHalfSize);	// Bottom left co-ords of block
	Vector2D blockP1 = (Vector2D) { blockPosition.x + blockHalfSize.x, blockPosition.y - blockHalfSize.y
	};	// Bottom right co-ords of block

	Vector2D ballTopLeftP0 = (Vector2D) { ballP0.x - ballHalfSize.x, ballP0.y + ballHalfSize.y };
	Vector2D ballTopLeftP1 = (Vector2D) { ballP1.x - ballHalfSize.x, ballP1.y + ballHalfSize.y };
	Vector2D ballTopRightP0 = (Vector2D) { ballP0.x + ballHalfSize.x, ballP0.y + ballHalfSize.y };
	Vector2D ballTopRightP1 = (Vector2D) { ballP1.x + ballHalfSize.x, ballP1.y + ballHalfSize.y };
	b32 doesIntersect = DoLineSegmentsIntersect(ballTopLeftP0, blockP0, ballTopLeftP1, blockP1)
		|| DoLineSegmentsIntersect(ballTopRightP0, blockP0, ballTopRightP1, blockP1);

	return doesIntersect;
}

internal b32 CollisionOnBrickTop(Vector2D blockHalfSize, Vector2D blockPosition, Vector2D ballHalfSize, Vector2D ballP0, Vector2D ballP1)
{
	Vector2D blockP0 = (Vector2D) { blockPosition.x - blockHalfSize.x, blockPosition.y + blockHalfSize.y };	// Top left co-ords of block
	Vector2D blockP1 = AddVector2D(blockPosition, blockHalfSize);	// Top right co-ords of block

	Vector2D ballBottomLeftP0 = (Vector2D) { ballP0.x - ballHalfSize.x, ballP0.y - ballHalfSize.y };
	Vector2D ballBottomLeftP1 = (Vector2D) { ballP1.x - ballHalfSize.x, ballP1.y - ballHalfSize.y };
	Vector2D ballBottomRightP0 = (Vector2D) { ballP0.x + ballHalfSize.x, ballP0.y - ballHalfSize.y };
	Vector2D ballBottomRightP1 = (Vector2D) { ballP1.x + ballHalfSize.x, ballP1.y - ballHalfSize.y };

	b32 doesIntersect = DoLineSegmentsIntersect(ballBottomLeftP0, blockP0, ballBottomLeftP1, blockP1)
		|| DoLineSegmentsIntersect(ballBottomRightP0, blockP0, ballBottomRightP1, blockP1);

	return doesIntersect;
}

internal b32 OnSegment(Vector2D p, Vector2D q, Vector2D r)
{
	if (q.x <= MaxFloat(p.x, r.x) && q.x >= MinFloat(p.x, r.x) &&
		q.y <= MaxFloat(p.y, r.y) && q.y >= MinFloat(p.y, r.y))
		return true;

	return false;
}

typedef enum Orientation
{
	Colinear = 0,
	Clockwise = 1,
	Counterclockwise = 2
} Orientation;

internal Orientation GetPointOrientation(Vector2D p, Vector2D q, Vector2D r)
{
	int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return Colinear;

	return (val > 0) ? Clockwise : Counterclockwise;
}

internal b32 DoLineSegmentsIntersect(Vector2D p1, Vector2D q1, Vector2D p2, Vector2D q2)
{
	Orientation o1 = GetPointOrientation(p1, q1, p2);
	Orientation o2 = GetPointOrientation(p1, q1, q2);
	Orientation o3 = GetPointOrientation(p2, q2, p1);
	Orientation o4 = GetPointOrientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;

		// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

	return false;
} 