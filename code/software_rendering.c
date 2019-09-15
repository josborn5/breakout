internal const float aspectRatio = 1.77f; // 16:9

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

internal void DrawRect(RenderBuffer* renderBuffer, uint32_t color, Vector2D halfSize, Vector2D p)
{
	float aspectRatioFactor = (float)renderBuffer->width;

	if (((float)renderBuffer->width / (float)renderBuffer->height) < aspectRatio)
	{
		aspectRatioFactor = (float)renderBuffer->height;
	}

	// factor half size for window size
	float scale = 0.001f;
	halfSize.x *= aspectRatioFactor * scale;
	halfSize.y *= aspectRatioFactor * scale;

	// factor position for window size
	p.x += aspectRatioFactor * scale;
	p.y += aspectRatioFactor * scale;

	p.x += (float)renderBuffer->width * .5f;
	p.y += (float)renderBuffer->height * .5f;
	
	int x0 = (int)(p.x - halfSize.x);
	int y0 = (int)(p.y - halfSize.y);
	int x1 = (int)(p.x + halfSize.x);
	int y1 = (int)(p.y + halfSize.y);

	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
}