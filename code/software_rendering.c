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
	x0 = Clamp(0, x0, renderBuffer->width);
	x1 = Clamp(0, x1, renderBuffer->width);
	y0 = Clamp(0, y0, renderBuffer->height);
	y1 = Clamp(0, y1, renderBuffer->height);

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

internal int TransformGameUnitPositionToPixelPosition(int input, float offset, float factor)
{
	int transformed = (int)(offset + (factor * input));
	return transformed;
}

internal void DrawRect(RenderBuffer* renderBuffer, uint32_t color, Vector2D halfSize, Vector2D p)
{
	float horizontalOffset = 0.5f * (float)renderBuffer->width;
	float verticalOffset = 0.5f * (float)renderBuffer->height;

	float currentAspectRatio = (float)renderBuffer->width / (float)renderBuffer->height;

	b32 factorToHeight = currentAspectRatio > BASE_ASPECT_RATIO;

	int smallestPixelDimension = (factorToHeight) ? renderBuffer->height : renderBuffer->width;
	int smallestUnitDimension = ((factorToHeight) ? Y_DIM_BASE : X_DIM_BASE )* 2;

	float scaleFactor = (float)smallestPixelDimension / (float)smallestUnitDimension;

	int xU0 = (int)(p.x - halfSize.x);
	int yU0 = (int)(p.y - halfSize.y);
	int xU1 = (int)(p.x + halfSize.x);
	int yU1 = (int)(p.y + halfSize.y);

	int xP0 = TransformGameUnitPositionToPixelPosition(xU0, horizontalOffset, scaleFactor);
	int yP0 = TransformGameUnitPositionToPixelPosition(yU0, verticalOffset, scaleFactor);
	int xP1 = TransformGameUnitPositionToPixelPosition(xU1, horizontalOffset, scaleFactor);
	int yP1 = TransformGameUnitPositionToPixelPosition(yU1, verticalOffset, scaleFactor);

	DrawRectInPixels(renderBuffer, color, xP0, yP0, xP1, yP1);
}