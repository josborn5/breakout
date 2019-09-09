internal int Clamp(int min, int val, int max)
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