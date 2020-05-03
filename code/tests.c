#include <stdio.h>

#include "world_transforms_tests.c"
#include "collision_tests.c"
#include "game_tests.c"

static int main()
{
	RunWorldTransformsTests();

	RunCollisionTests();

	RunGameTests();

	printf("Press any key to continue...");
	getchar();
}