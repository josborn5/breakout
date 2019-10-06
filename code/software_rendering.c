internal const int X_DIM_BASE = 160;
internal const int Y_DIM_BASE = 90;
#define BASE_ASPECT_RATIO (X_DIM_BASE / Y_DIM_BASE)

internal void ClearScreen(RenderBuffer* renderBuffer, uint32_t color)
{
	uint32_t *pixel = renderBuffer->pixels;

	for (int y = 0; y < renderBuffer->height; y += 1)
	{
		for (int x = 0; x< renderBuffer->width; x += 1)
		{
			*pixel = color;
			pixel++;
		}
	}
}

internal void DrawRectInPixels(RenderBuffer* renderBuffer, uint32_t color, int x0, int y0, int x1, int y1)
{
	// Make sure writing to the render buffer does not escape its bounds
	x0 = ClampInt(0, x0, renderBuffer->width);
	x1 = ClampInt(0, x1, renderBuffer->width);
	y0 = ClampInt(0, y0, renderBuffer->height);
	y1 = ClampInt(0, y1, renderBuffer->height);

	for (int y = y0; y < y1; y++)
	{
		int positionStartOfRow = renderBuffer->width * y;
		int positionStartOfX0InRow = positionStartOfRow + x0;
		uint32_t* pixel = renderBuffer->pixels + positionStartOfX0InRow;
		for (int x = x0; x < x1; x++)
		{
			*pixel = color;
			pixel++;
		}
	}
}

internal int TransformGamePositionToPixelPosition(int input, float offset, float factor)
{
	int transformed = (int)(offset + (factor * input));
	return transformed;
}

internal void GetPixelAndGameDimensions(RenderBuffer* renderBuffer, int* smallestPixelDimension, int* smallestUnitDimension)
{
	float currentAspectRatio = (float)renderBuffer->width / (float)renderBuffer->height;

	b32 factorToHeight = currentAspectRatio > BASE_ASPECT_RATIO;

	*smallestPixelDimension = (factorToHeight) ? renderBuffer->height : renderBuffer->width;
	*smallestUnitDimension = ((factorToHeight) ? Y_DIM_BASE : X_DIM_BASE ) * 2;
}

internal Vector2D TransformPixelCoordToGameCoord(RenderBuffer* renderBuffer, int x, int y)
{
	float horizontalOffset = 0.5f * (float)renderBuffer->width;
	float verticalOffset = 0.5f * (float)renderBuffer->height;

	int smallestPixelDimension;
	int smallestUnitDimension;
	GetPixelAndGameDimensions(renderBuffer, &smallestPixelDimension, &smallestUnitDimension);

	float scaleFactor = (float)smallestUnitDimension / (float)smallestPixelDimension;

	Vector2D transformed = {0};
	transformed.x = (x - horizontalOffset) * scaleFactor;
	transformed.y = (y - verticalOffset) * scaleFactor;

	return transformed;
}

internal void TranformVectorsToPixels(RenderBuffer* renderBuffer, Vector2D halfSize, Vector2D p, int* x0, int* y0, int* x1, int* y1)
{
	float horizontalOffset = 0.5f * (float)renderBuffer->width;
	float verticalOffset = 0.5f * (float)renderBuffer->height;

	int smallestPixelDimension;
	int smallestUnitDimension;
	GetPixelAndGameDimensions(renderBuffer, &smallestPixelDimension, &smallestUnitDimension);

	float scaleFactor = (float)smallestPixelDimension / (float)smallestUnitDimension;

	int xU0 = (int)(p.x - halfSize.x);
	int yU0 = (int)(p.y - halfSize.y);
	int xU1 = (int)(p.x + halfSize.x);
	int yU1 = (int)(p.y + halfSize.y);

	*x0 = TransformGamePositionToPixelPosition(xU0, horizontalOffset, scaleFactor);
	*y0 = TransformGamePositionToPixelPosition(yU0, verticalOffset, scaleFactor);
	*x1 = TransformGamePositionToPixelPosition(xU1, horizontalOffset, scaleFactor);
	*y1 = TransformGamePositionToPixelPosition(yU1, verticalOffset, scaleFactor);
}

internal void DrawRect(RenderBuffer* renderBuffer, uint32_t color, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	TranformVectorsToPixels(renderBuffer, halfSize, p, &x0, &y0, &x1, &y1);

	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
}

internal void ClearScreenAndDrawRect(RenderBuffer* renderBuffer, uint32_t color, uint32_t clearColor, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	TranformVectorsToPixels(renderBuffer, halfSize, p, &x0, &y0, &x1, &y1);

	// draw the given rectangle
	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);

	// draw rectangles around the given rectangle to clear the background
	DrawRectInPixels(renderBuffer, clearColor, 0, 0, x0, renderBuffer->height);						// left of rect
	DrawRectInPixels(renderBuffer, clearColor, x1, 0, renderBuffer->width, renderBuffer->height);	// right of rect
	DrawRectInPixels(renderBuffer, clearColor, x0, 0, x1, y0);									// above rect
	DrawRectInPixels(renderBuffer, clearColor, x0, y1, x1, renderBuffer->height);					// below rect
}
