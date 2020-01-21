#include "math.h"

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


internal void DrawRect(RenderBuffer* renderBuffer, Rect gameRect, uint32_t color, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	Rect pixelRect = (Rect) { renderBuffer->width, renderBuffer->height };
	TranformVectorsToPixels(pixelRect, gameRect, halfSize, p, &x0, &y0, &x1, &y1);

	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
}

internal void ClearScreenAndDrawRect(RenderBuffer* renderBuffer, Rect gameRect, uint32_t color, uint32_t clearColor, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	Rect pixelRect = (Rect) { renderBuffer->width, renderBuffer->height };
	TranformVectorsToPixels(pixelRect, gameRect, halfSize, p, &x0, &y0, &x1, &y1);

	// draw the given rectangle
	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);

	// draw rectangles around the given rectangle to clear the background
	DrawRectInPixels(renderBuffer, clearColor, 0, 0, x0, renderBuffer->height);						// left of rect
	DrawRectInPixels(renderBuffer, clearColor, x1, 0, renderBuffer->width, renderBuffer->height);	// right of rect
	DrawRectInPixels(renderBuffer, clearColor, x0, 0, x1, y0);									// above rect
	DrawRectInPixels(renderBuffer, clearColor, x0, y1, x1, renderBuffer->height);					// below rect
}
