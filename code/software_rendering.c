#include "math.h"
#include "../../win32-platform/bin/platform.hpp"
#include "../../win32-platform/bin/math.hpp"

static void ClearScreen(const RenderBuffer &renderBuffer, uint32_t color)
{
	uint32_t *pixel = renderBuffer.pixels;

	for (int y = 0; y < renderBuffer.height; y += 1)
	{
		for (int x = 0; x< renderBuffer.width; x += 1)
		{
			*pixel = color;
			pixel++;
		}
	}
}

static void DrawRectInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int y0, int x1, int y1)
{
	// Make sure writing to the render buffer does not escape its bounds
	x0 = ClampInt(1, x0, renderBuffer.width);
	x1 = ClampInt(1, x1, renderBuffer.width);
	y0 = ClampInt(1, y0, renderBuffer.height);
	y1 = ClampInt(1, y1, renderBuffer.height);

	for (int y = y0; y < y1; y++)
	{
		int positionStartOfRow = renderBuffer.width * y;
		int positionStartOfX0InRow = positionStartOfRow + x0;
		uint32_t* pixel = renderBuffer.pixels + positionStartOfX0InRow;
		for (int x = x0; x < x1; x++)
		{
			*pixel = color;
			pixel++;
		}
	}
}

static void PlotPixel(const RenderBuffer &renderBuffer, uint32_t color, int x, int y)
{
	int positionStartOfRow = renderBuffer.width * y;
	int positionStartOfX0InRow = positionStartOfRow + x;
	uint32_t* pixel = renderBuffer.pixels + positionStartOfX0InRow;
	*pixel = color;
}

static void DrawVerticalLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x, int y0, int y1)
{
	int yDiff = y1 - y0;
	int yDiffMod = (yDiff < 0) ? -1 * yDiff : yDiff;
	int yIncrement = (yDiff < 0) ? -1 : 1;
	PlotPixel(renderBuffer, color, x, y0);
	for (int i = 0; i <= yDiffMod; i += 1)
	{
		y0 += yIncrement;
		PlotPixel(renderBuffer, color, x, y0);
	}
}

static void DrawHorizontalLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int x1, int y)
{
	int xDiff = x1 - x0;
	int xDiffMod = (xDiff < 0) ? -1 * xDiff : xDiff;
	int xIncrement = (xDiff < 0) ? -1 : 1;
	PlotPixel(renderBuffer, color, x0, y);
	for (int i = 0; i < xDiffMod; i += 1)
	{
		x0 += xIncrement;
		PlotPixel(renderBuffer, color, x0, y);
	}
}

// Implemented with Bresenham's algorithm
static void DrawLineInPixels(const RenderBuffer &renderBuffer, uint32_t color, int x0, int y0, int x1, int y1)
{
	// Make sure writing to the render buffer does not escape its bounds
	x0 = ClampInt(1, x0, renderBuffer.width);
	x1 = ClampInt(1, x1, renderBuffer.width);
	y0 = ClampInt(1, y0, renderBuffer.height);
	y1 = ClampInt(1, y1, renderBuffer.height);
	
	int xDiff = x1 - x0;
	if (xDiff == 0)
	{
		DrawVerticalLineInPixels(renderBuffer, color, x0, y0, y1);
		return;
	}

	int yDiff = y1 - y0;
	if (yDiff == 0)
	{
		DrawHorizontalLineInPixels(renderBuffer, color, x0, x1, y0);
		return;
	}

	int xDiffMod = (xDiff < 0) ? -1 * xDiff : xDiff;
	int yDiffMod = (yDiff < 0) ? -1 * yDiff : yDiff;
	int modDiff = yDiffMod - xDiffMod;
	int xIncrement = (xDiff < 0) ? -1 : 1;
	int yIncrement = (yDiff < 0) ? -1 : 1;

	PlotPixel(renderBuffer, color, x0, y0);
	// If the gradient is 1 simply increment both X & Y at on every iteration
	if (modDiff == 0)
	{
		for (int i = 0; i < xDiffMod; ++i)
		{
			x0 += xIncrement;
			y0 += yIncrement;
			PlotPixel(renderBuffer, color, x0, y0);
		}
		return;
	}

	// If the gradient is more than one then y gets incremented on every step along the line and x sometimes gets incremented
	// If the gradient is less than one then x gets incremented on every step along the line and y sometimes gets incremented
	bool isLongDimensionX = (modDiff < 0);
	int longDimensionDiff;
	int* longDimensionVar;	// Make this a pointer so PlotPixel can still be called with x0 & y0 arguments
	int longDimensionIncrement;
	int shortDimensionDiff;
	int* shortDimensionVar;	// Make this a pointer so PlotPixel can still be called with x0 & y0 arguments
	int shortDimensionIncrement;

	if (isLongDimensionX)
	{
		longDimensionDiff = xDiffMod;
		longDimensionVar = &x0;
		longDimensionIncrement = xIncrement;

		shortDimensionDiff = yDiffMod;
		shortDimensionVar = &y0;
		shortDimensionIncrement = yIncrement;
	}
	else
	{
		longDimensionDiff = yDiffMod;
		longDimensionVar = &y0;
		longDimensionIncrement = yIncrement;

		shortDimensionDiff = xDiffMod;
		shortDimensionVar = &x0;
		shortDimensionIncrement = xIncrement;
	}

	int p = (2 * xDiffMod) - yDiffMod;
	int negativePIncrement = 2 * shortDimensionDiff;
	int positivePIncrement = negativePIncrement - (2 * longDimensionDiff);

	for (int i = 1; i < longDimensionDiff; i += 1)
	{
		*longDimensionVar += longDimensionIncrement;
		if (p < 0)
		{
			p += negativePIncrement;
		}
		else
		{
			p += positivePIncrement;
			*shortDimensionVar += shortDimensionIncrement;
		}
		PlotPixel(renderBuffer, color, x0, y0);
	}
}

static void DrawRect(const RenderBuffer &renderBuffer, Rect gameRect, uint32_t color, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	Rect pixelRect = {0};
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;
	TranformVectorsToPixels(pixelRect, gameRect, halfSize, p, &x0, &y0, &x1, &y1);

	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
}

static void DrawRect(const RenderBuffer &renderBuffer, Rect gameRect, uint32_t color, const gentle::Vec2<float> &halfSize, const gentle::Vec2<float> &p)
{
	Vector2D tempHalfSize = Vector2D { halfSize.x, halfSize.y };
	Vector2D tempP = Vector2D { p.x, p.y };
	DrawRect(renderBuffer, gameRect, color, tempHalfSize, tempP);
}

static void ClearScreenAndDrawRect(const RenderBuffer &renderBuffer, Rect gameRect, uint32_t color, uint32_t clearColor, Vector2D halfSize, Vector2D p)
{
	int x0, y0, x1, y1;
	Rect pixelRect = {0};
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;
	TranformVectorsToPixels(pixelRect, gameRect, halfSize, p, &x0, &y0, &x1, &y1);

	// draw the given rectangle
	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);

	// draw rectangles around the given rectangle to clear the background
	DrawRectInPixels(renderBuffer, clearColor, 0, 0, x0, renderBuffer.height);						// left of rect
	DrawRectInPixels(renderBuffer, clearColor, x1, 0, renderBuffer.width, renderBuffer.height);	// right of rect
	DrawRectInPixels(renderBuffer, clearColor, x0, 0, x1, y0);										// above rect
	DrawRectInPixels(renderBuffer, clearColor, x0, y1, x1, renderBuffer.height);					// below rect
}

static void DrawSprite(const RenderBuffer &renderBuffer, Rect gameRect, char *sprite, Vector2D p, float blockHalfSize, uint32_t color)
{
	float originalX = p.x;
	float originalY = p.y;

	float blockSize = blockHalfSize * 2.0f;
	Vector2D blockHalf;
	blockHalf.x = blockHalfSize;
	blockHalf.y = blockHalfSize;

	while (*sprite)
	{
		if (*sprite == '\n')
		{
			p.y -= blockSize;	// We're populating blocks in the sprint left to right, top to bottom. So y is decreasing.
			p.x = originalX; // reset cursor to start of next row
		}
		else
		{
			if (*sprite != ' ')
			{
				DrawRect(renderBuffer, gameRect, color, blockHalf, p);
			}
			p.x += blockSize;
		}
		sprite++;
	}

	p.x = originalX;
	p.y = originalY;
}

// Render characters
const float CHARACTER_HEIGHT = 7.0f;
char *letters[26] = {
"\
 00\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0",

"\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
000",

"\
 000\n\
0\n\
0\n\
0\n\
0\n\
0\n\
 000",

"\
000\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
000",

"\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0000",

"\
0000\n\
0\n\
0\n\
000\n\
0\n\
0\n\
0",

"\
 000\n\
0\n\
0\n\
0 00\n\
0  0\n\
0  0\n\
 000",

"\
0  0\n\
0  0\n\
0  0\n\
0000\n\
0  0\n\
0  0\n\
0  0",

"\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
000",

"\
 000\n\
   0\n\
   0\n\
   0\n\
0  0\n\
0  0\n\
 000",

"\
0  0\n\
0  0\n\
0 0\n\
00\n\
0 0\n\
0  0\n\
0  0",

"\
0\n\
0\n\
0\n\
0\n\
0\n\
0\n\
0000",

"\
00 00\n\
0 0 0\n\
0 0 0\n\
0   0\n\
0   0\n\
0   0\n\
0   0",

"\
0   0\n\
00  0\n\
0 0 0\n\
0 0 0\n\
0 0 0\n\
0  00\n\
0   0",

"\
 00 \n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00",

"\
000\n\
0  0\n\
0  0\n\
000\n\
0\n\
0\n\
0",

"\
 000\n\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0  0\n\
 00 0",

"\
000\n\
0  0\n\
0  0\n\
000\n\
0  0\n\
0  0\n\
0  0",

"\
 000\n\
0\n\
0\n\
 00\n\
   0\n\
   0\n\
000",

"\
000\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0\n\
 0",

"\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
0  0\n\
 00",

"\
0   0\n\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0",

"\
0   0\n\
0   0\n\
0   0\n\
0 0 0\n\
0 0 0\n\
 0 0\n\
 0 0",

"\
0   0\n\
0   0\n\
 0 0\n\
  0\n\
 0 0\n\
0   0\n\
0   0",

"\
0   0\n\
0   0\n\
 0 0\n\
 0 0\n\
  0\n\
  0\n\
  0",

"\
0000\n\
   0\n\
  0\n\
 0\n\
0\n\
0\n\
0000"
};

char *digits[10] = {
"\
 000 \n\
0   0\n\
0  00\n\
0 0 0\n\
00  0\n\
0   0\n\
 000",

"\
 0\n\
00\n\
 0\n\
 0\n\
 0\n\
 0\n\
000",

"\
 00\n\
0  0\n\
   0\n\
  0\n\
 0\n\
0\n\
0000",

"\
 00\n\
0  0\n\
   0\n\
 00\n\
   0\n\
0  0\n\
 00",

"\
  00\n\
 0 0\n\
0  0\n\
0000\n\
   0\n\
   0\n\
   0",

"\
0000\n\
0\n\
0\n\
000\n\
   0\n\
   0\n\
000",

"\
 000\n\
0\n\
0\n\
000\n\
0  0\n\
0  0\n\
 00",

"\
0000\n\
   0\n\
   0\n\
  0\n\
 0\n\
0\n\
0",

"\
 00\n\
0  0\n\
0  0\n\
 00\n\
0  0\n\
0  0\n\
 00",

"\
 00\n\
0  0\n\
0  0\n\
 00\n\
  0\n\
 0\n\
0"
};

int GetLetterIndex(char c)
{
	return c - 'A';
}

static void DrawAlphabetCharacters(const RenderBuffer &renderBuffer, Rect gameRect, char *text, Vector2D p, float fontSize, uint32_t color)
{
	float blockHalfSize = fontSize / (2.0f * CHARACTER_HEIGHT);
	float characterWidth = fontSize;
	float originalX = p.x;
	
	for (char *letterAt = text; *letterAt; letterAt++)
	{
		if (*letterAt != ' ')
		{
			int letterIndex = GetLetterIndex(*letterAt);
			char *letter = letters[letterIndex];

			DrawSprite(renderBuffer, gameRect, letter, p, blockHalfSize, color);
		}
		p.x += characterWidth;
	}

	p.x = originalX;
}

static void DrawNumber(const RenderBuffer &renderBuffer, Rect gameRect, int number, Vector2D p, float fontSize, uint32_t color)
{
	float blockHalfSize = fontSize / (2.0f * CHARACTER_HEIGHT);
	float characterWidth = fontSize;
	float originalX = p.x;

	int baseTenMultiplier = 1;
	int digit = number / baseTenMultiplier;
	int digitCount = 1;
	while (digit > 0)
	{
		baseTenMultiplier *= 10;
		digitCount += 1;
		digit = number / baseTenMultiplier;
	}

	int workingNumber = number;
	while (baseTenMultiplier >= 10)
	{
		baseTenMultiplier /= 10;
		digit = workingNumber / baseTenMultiplier;

		workingNumber -= (digit * baseTenMultiplier);

		char *charDigit = digits[digit];
		DrawSprite(renderBuffer, gameRect, charDigit, p, blockHalfSize, color);

		p.x += characterWidth;
	}

	p.x = originalX;
}
