#include "game.h"
#include "../../win32-platform/bin/math.hpp"

const gentle::Vec2<float> POWER_UP_VELOCITY = gentle::Vec2<float> { 0.0f, -50.0f };
const gentle::Vec2<float> POWER_UP_HALF_SIZE = gentle::Vec2<float> { 5.0f, 5.0f };

char *levels[10] = {
"\
000000000000\n\
000000000000\n\
MMMMMMMMMMMM",

"\
     00\n\
    0000\n\
  00000000\n\
CCCCCCCCCCCC",

"\
000000000000\n\
  00C00C00\n\
    0000\n\
     MM"
};

static void PopulateBlocksForLevel(int level, Block* block, int blockArraySize, gentle::Vec2<float> blockArea, gentle::Vec2<float> blockAreaPosition)
{
	// clear out any remaining blocks in the block array
	Block* firstBlock = block;
	gentle::Vec2<float> originVector = gentle::Vec2<float> { 0.0f, 0.0f };
	for (int i = 0; i < blockArraySize; i += 1)
	{
		firstBlock->exists = 0;
		firstBlock->halfSize = originVector;
		firstBlock->position = originVector;
		firstBlock->color = 0;
		firstBlock++;
	}

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
	if (columnCounter > columnCount)
	{
		columnCount = columnCounter;
	}

	// Calculate the block size from the row & column counts
	float blockHeight = blockArea.y / (float)rowCount;
	float blockWidth = blockArea.x / (float)columnCount;
	gentle::Vec2<float> blockHalfSize;
	blockHalfSize.x = 0.5f * blockWidth;
	blockHalfSize.y = 0.5f * blockHeight;

	int blockCount = 0;
	gentle::Vec2<float> blockPosition = blockAreaPosition;
	blockPosition.x += blockHalfSize.x;
	blockPosition.y -= blockHalfSize.y;
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
				block->color = MakeColorFromGrey((uint8_t)(blockCount * 20));
				switch (*blockLayoutForLevel)
				{
					case 'M':
						block->powerUp.type = Multiball;
						block->powerUp.position = gentle::Vec2<float> { block->position.x, block->position.y };
						block->powerUp.halfSize = POWER_UP_HALF_SIZE;
						block->powerUp.velocity = POWER_UP_VELOCITY;
						block->powerUp.exists = false;
						block->powerUp.color = 0xFF0000;
						break;
					case 'C':
						block->powerUp.type = Comet;
						block->powerUp.position = gentle::Vec2<float> { block->position.x, block->position.y };
						block->powerUp.halfSize = POWER_UP_HALF_SIZE;
						block->powerUp.velocity = POWER_UP_VELOCITY;
						block->powerUp.exists = false;
						block->powerUp.color = 0xFFFF00;
						break;
					default:
						block->powerUp.type = Nothing;
						block->powerUp.position = gentle::Vec2<float> { 0.0f, 0.0f };
						block->powerUp.halfSize = gentle::Vec2<float> { 0.0f, 0.0f };
						block->powerUp.velocity = gentle::Vec2<float> { 0.0f, 0.0f };
						block->powerUp.exists = false;
						block->powerUp.color = 0x000000;
						break;
				}

				blockCount += 1;
			}

			blockPosition.x += blockWidth;
		}

		block++;
		blockLayoutForLevel++;
	}
}
