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
	DrawRectInPixels(renderBuffer, clearColor, x0, 0, x1, y0);										// above rect
	DrawRectInPixels(renderBuffer, clearColor, x0, y1, x1, renderBuffer->height);					// below rect
}

static void DrawSprite(RenderBuffer* renderBuffer, Rect gameRect, char *sprite, Vector2D p, float blockHalfSize, uint32_t color)
{
	float originalX = p.x;
	float originalY = p.y;

	float blockSize = blockHalfSize * 2.0f;
	Vector2D blockHalf = (Vector2D){ blockHalfSize, blockHalfSize };

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

static void DrawAlphabetCharacters(RenderBuffer* renderBuffer, Rect gameRect, char *text, Vector2D p, float fontSize, uint32_t color)
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

static void DrawNumber(RenderBuffer* renderBuffer, Rect gameRect, int number, Vector2D p, float fontSize, uint32_t color)
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
