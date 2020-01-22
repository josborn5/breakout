#include "math.h"

int TransformGamePositionToPixelPosition(int input, float offset, float factor)
{
	int transformed = (int)(offset + (factor * input));
	return transformed;
}

Vector2D TransformPixelCoordToGameCoord(Rect pixelRect, Rect gameRect, int x, int y)
{
	float mX;
	float mY;
	float cX;
	float cY;

	// Check the aspect ratio of the render buffer
	float pixelAspectRatio = (float)pixelRect.x / (float)pixelRect.y;
	float gameAspectRatio = gameRect.aspectRatio;

	if (pixelAspectRatio == gameAspectRatio)
	{
		mX = (float)gameRect.x / (float)pixelRect.x;
		mY = (float)gameRect.y / (float)pixelRect.y;
	}
	else if (pixelAspectRatio > gameAspectRatio)
	{
		mX = ((float)gameRect.x * pixelAspectRatio) / ((float)pixelRect.x * gameAspectRatio);
		mY = (float)gameRect.y / (float)pixelRect.y;
	}
	else
	{
		mX = (float)gameRect.x / (float)pixelRect.x;
		mY = ((float)gameRect.y * gameAspectRatio) / ((float)pixelRect.y * pixelAspectRatio);
	}

	float horizontalOffset = 0.0f;
	float verticalOffset = 0.0f;
	Vector2D transformed = {0};
	transformed.x = (x - horizontalOffset) * mX;
	transformed.y = (y - verticalOffset) * mY;

	return transformed;
}

Rect TransformGameCoordToPixelCoord(Rect pixelRect, Rect gameRect, float x, float y)
{
	float mX;
	float mY;
	float cX;
	float cY;

	// Check the aspect ratio of the render buffer
	float pixelAspectRatio = (float)pixelRect.x / (float)pixelRect.y;
	float gameAspectRatio = gameRect.aspectRatio;

	if (pixelAspectRatio == gameAspectRatio)
	{
		mX = (float)pixelRect.x / (float)gameRect.x;
		mY = (float)pixelRect.y / (float)gameRect.y;
	}
	else if (pixelAspectRatio > gameAspectRatio)
	{
		mX = ((float)pixelRect.x * gameAspectRatio) / ((float)gameRect.x * pixelAspectRatio);
		mY = (float)pixelRect.y / (float)gameRect.y;
	}
	else
	{
		mX = (float)pixelRect.x / (float)gameRect.x;
		mY = ((float)pixelRect.y * pixelAspectRatio) / ((float)gameRect.y * gameAspectRatio);
	}

	float horizontalOffset = 0.0f;
	float verticalOffset = 0.0f;
	Rect transformed = {0};
	transformed.x = (int)((x - horizontalOffset) * mX);
	transformed.y = (int)((y - verticalOffset) * mY);

	return transformed;
}

void TranformVectorsToPixels(Rect pixelRect, Rect gameRect, Vector2D halfSize, Vector2D p, int* x0, int* y0, int* x1, int* y1)
{
	float game0x = p.x - halfSize.x;
	float game0y = p.y - halfSize.y;
	float game1x = p.x + halfSize.x;
	float game1y = p.y + halfSize.y;
	Rect pixel0 = TransformGameCoordToPixelCoord(pixelRect, gameRect, game0x, game0y);
	Rect pixel1 = TransformGameCoordToPixelCoord(pixelRect, gameRect, game1x, game1y);

	*x0 = pixel0.x;
	*y0 = pixel0.y;
	*x1 = pixel1.x;
	*y1 = pixel1.y;
}
