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