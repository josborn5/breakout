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

internal uint32_t MakeColorFromGrey(uint8_t grey)
{
	return (grey << 0) | (grey << 8) | (grey << 16);
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
