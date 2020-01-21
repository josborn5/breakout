#include <stdio.h>
#include <assert.h>

#include "world_transforms.c"

static int main()
{
	/**
	 *
	 * TransformPixelCoordToGameCoord
	 *
	 */
	// pixel aspect ratio same as game aspect ratio
	Rect pixelRect = (Rect) { 100, 100 };
	Rect gameRect = (Rect) { 100, 100, 1 };
	Vector2D vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	assert(vectorResult.x == 50);
	assert(vectorResult.y == 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);

	// game aspect ratio greater than pixel aspect ratio
	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 200, 100, 2 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 50, 2 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	assert(vectorResult.x == 50);
	assert(vectorResult.y == 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 200, 100, 2 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 100, 25);
	assert(vectorResult.x == 200);
	assert(vectorResult.y == 50);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 50, 2 };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 100, 100);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);

	// game aspect ratio less than pixel aspect ratio
	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 200, 0.5f };
	vectorResult = TransformPixelCoordToGameCoord(pixelRect, gameRect, 50, 50);
	assert(vectorResult.x == 100);
	assert(vectorResult.y == 100);
	printf("x is %f\n", vectorResult.x);
	printf("y is %f\n\n", vectorResult.y);


	/**
	 *
	 * TranformVectorsToPixels
	 *
	 */
	// pixel aspect ratio same as game aspect ratio
	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 100, 1 };
	Rect rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 50, 50);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);

	// game aspect ratio greater than pixel aspect ratio
	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 200, 100, 2 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 50, 2 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 50, 50);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 200, 100, 2 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 200, 50);
	assert(rectResult.x == 100);
	assert(rectResult.y == 25);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);

	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 50, 2 };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	assert(rectResult.x == 100);
	assert(rectResult.y == 100);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);

	// game aspect ratio less than pixel aspect ratio
	pixelRect = (Rect) { 100, 100 };
	gameRect = (Rect) { 100, 200, 0.5f };
	rectResult = TransformGameCoordToPixelCoord(pixelRect, gameRect, 100, 100);
	assert(rectResult.x == 50);
	assert(rectResult.y == 50);
	printf("x is %d\n", rectResult.x);
	printf("y is %d\n\n", rectResult.y);


	printf("Press any key to continue...");
	getchar();
}