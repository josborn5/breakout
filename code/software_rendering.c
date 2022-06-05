#include "../../win32-platform/bin/platform.hpp"
#include "../../win32-platform/bin/math.hpp"


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


// !!! https://youtu.be/Mi98zVBb6Wk?list=PLnuhp3Xd9PYTt6svyQPyRO_AAuMWGxPzU&t=2865
static void DrawRect(const RenderBuffer &renderBuffer, const gentle::Vec2<int> &gameRect, uint32_t color, const gentle::Vec2<float> &halfSize, const gentle::Vec2<float> &p)
{
	int x0 = (int)p.x - (int)halfSize.x;
	int x1 = (int)p.x + (int)halfSize.x;
	int y0 = (int)p.y - (int)halfSize.y;
	int y1 = (int)p.y + (int)halfSize.y;

	DrawRectInPixels(renderBuffer, color, x0, y0, x1, y1);
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

static void DrawAlphabetCharacters(const RenderBuffer &renderBuffer, const gentle::Vec2<int> &gameRect, char *text, const gentle::Vec2<float> &p, float fontSize, uint32_t color)
{
	float blockHalfSize = fontSize / (2.0f * CHARACTER_HEIGHT);
	float characterWidth = fontSize;
	gentle::Vec2<float> pCopy = gentle::Vec2<float> { p.x, p.y };
	
	for (char *letterAt = text; *letterAt; letterAt++)
	{
		if (*letterAt != ' ')
		{
			int letterIndex = GetLetterIndex(*letterAt);
			char *letter = letters[letterIndex];

			gentle::DrawSprite(renderBuffer, letter, pCopy, blockHalfSize, color);
		}
		pCopy.x += characterWidth;
	}
}

static void DrawNumber(const RenderBuffer &renderBuffer, const gentle::Vec2<int> &gameRect, int number, const gentle::Vec2<float> &p, float fontSize, uint32_t color)
{
	float blockHalfSize = fontSize / (2.0f * CHARACTER_HEIGHT);
	float characterWidth = fontSize;
	gentle::Vec2<float> pCopy = gentle::Vec2<float> { p.x, p.y };

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
		gentle::DrawSprite(renderBuffer, charDigit, pCopy, blockHalfSize, color);

		pCopy.x += characterWidth;
	}
}
