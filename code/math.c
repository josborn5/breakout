internal int ClampInt(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

internal float ClampFloat(float min, float val, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

// Vector 2d
struct
{
	union
	{
		struct
		{
			float x;
			float y;
		};

		float e[2];
	};
} typedef Vector2D;

internal Vector2D AddVector2D(Vector2D a, Vector2D b)
{
	return (Vector2D){ a.x + b.x, a.y + b.y };
}

internal Vector2D SubtractVector2D(Vector2D a, Vector2D b)
{
	return (Vector2D){ a.x - b.x, a.y - b.y };
}

internal Vector2D MultiplyVector2D(Vector2D a, float scalar)
{
	return (Vector2D){ scalar * a.x, scalar * a.y };
}

internal b32 AABBCollideRectToRect(Vector2D aHalfSize, Vector2D aPos, Vector2D bHalfSize, Vector2D bPos)
{
	Vector2D b0 = SubtractVector2D(bPos, bHalfSize);	// Bottom left co-ords of b
	Vector2D b1 = AddVector2D(bPos, bHalfSize);			// Top rght co-ords of b
	Vector2D a0 = SubtractVector2D(aPos, aHalfSize);	// Bottom left co-ords of a
	Vector2D a1 = AddVector2D(aPos, aHalfSize);			// Top right co-ords of a
	b32 verticalCollision = (b0.y < a1.y && b1.y > a0.y);
	b32 horizontalCollision = (b0.x < a1.x && b1.x > a0.x);
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