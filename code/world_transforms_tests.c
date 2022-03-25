#include <stdio.h>
#include <assert.h>

#include "world_transforms.c"
#include "../../win32-platform/bin/math.hpp"

static void RunWorldTransformsTests()
{
	/**
	 *
	 * TransformPixelCoordToGameCoord
	 *
	 */
	// pixel aspect ratio same as game aspect ratio
	gentle::Vec2<int> pixelRect = { 1280, 720 };
	gentle::Vec2<int> gameRect = { 160, 90 };
	gentle::Vec2<float> vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 1280, 720);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 160);
	assert(vectorResult.y == 90);

	pixelRect = { 1280, 720 };
	gameRect = { 160, 90 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 1280, 720);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 160);
	assert(vectorResult.y == 90);

	pixelRect = { 1280, 720 };
	gameRect = { 160, 90 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 1279, 719);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 159.875);
	assert(vectorResult.y == 89.875);

	// game aspect ratio greater than pixel aspect ratio
	pixelRect = { 100, 100 };
	gameRect = { 200, 100 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);

	pixelRect = { 100, 100 };
	gameRect = { 100, 50 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 50);
	assert(vectorResult.y == 50);

	pixelRect = { 100, 100 };
	gameRect = { 200, 100 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 100, 25);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 200);
	assert(vectorResult.y == 50);

	pixelRect = { 100, 100 };
	gameRect = { 100, 50 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 100, 100);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);

	// game aspect ratio less than pixel aspect ratio
	pixelRect = { 100, 100 };
	gameRect = { 100, 200 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);


	/**
	 *
	 * TranformVectorsToPixels
	 *
	 */
	// pixel aspect ratio same as game aspect ratio
	pixelRect = { 100, 100 };
	gameRect = { 100, 100 };
	gentle::Vec2<int> rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 50, 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);

	pixelRect = { 1280, 720 };
	gameRect = { 160, 90 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 160, 90);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 1280);
	assert(rectResult.y == 720);

	pixelRect = { 1280, 720 };
	gameRect = { 160, 90 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 159.875, 89.875);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 1279);
	assert(rectResult.y == 719);

	// game aspect ratio greater than pixel aspect ratio
	pixelRect = { 100, 100 };
	gameRect = { 200, 100 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);

	pixelRect = { 100, 100 };
	gameRect = { 100, 50 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 50, 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);

	pixelRect = { 100, 100 };
	gameRect = { 200, 100 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 200, 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 100);
	assert(rectResult.y == 25);

	pixelRect = { 100, 100 };
	gameRect = { 100, 50 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 100);
	assert(rectResult.y == 100);

	// game aspect ratio less than pixel aspect ratio
	pixelRect = { 100, 100 };
	gameRect = { 100, 200 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);
}
