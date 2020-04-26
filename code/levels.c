#include "math.h"
#include "game.h"

char *levels[10] = {
"\
00000000\n\
00000000",

"\
   00\n\
  0000\n\
00000000",

"\
00000000\n\
  0000\n\
   00"
};

static void PopulateBlocksForLevel(int level, Block* block, int blockArraySize, Vector2D blockArea, Vector2D blockAreaPosition)
{
	// Temporary hack - force level to be between 1 and 3
	if (level > 3)
	{
		level = 1;
	}

	char* blockLayoutForLevel = levels[level - 1];

	// count the newlines to work out how many rows of blocks we have
	char* firstCharacter = blockLayoutForLevel; // keep a reference to the first character so we can iterate through it again

	int rowCount = 1;
	int columnCounter = 0;
	int columnCount = 0;
	while (*firstCharacter)
	{
		if (*firstCharacter == '\n')
		{
			rowCount += 1;
			if (columnCounter > columnCount)
			{
				columnCount = columnCounter;
			}
			columnCounter = 0;
		}
		else
		{
			columnCounter += 1;
		}
		firstCharacter++;
	}

	// Calculate the block size from the row & column counts
	float blockHeight = blockArea.y / (float)rowCount;
	float blockWidth = blockArea.x / (float)columnCount;
	Vector2D blockHalfSize = (Vector2D){ 0.5f * blockWidth, 0.5f * blockHeight };

	int blockCount = 0;
	Vector2D blockPosition = blockAreaPosition;
	float originalX = blockPosition.x;
	while (*blockLayoutForLevel)
	{
		if (*blockLayoutForLevel == '\n')
		{
			blockPosition.x = originalX;
			blockPosition.y -= blockHeight;
		}
		else
		{
			if (*blockLayoutForLevel != ' ')
			{
				block->exists = 1;
				block->halfSize = blockHalfSize;
				block->position = blockPosition;
				block->color = MakeColorFromGrey(blockCount * 20);

				blockCount += 1;
			}

			blockPosition.x += blockWidth;
		}

		block++;
		blockLayoutForLevel++;
	}
}
