#ifndef MATH_H
#define MATH_H

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

#endif