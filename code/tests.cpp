#include <stdio.h>

#include "world_transforms_tests.c"
#include "collision_tests.c"
#include "math_tests.c"

int main()
{
	RunWorldTransformsTests();

	RunCollisionTests();

	RunGameTests();

	printf("Press any key to continue...");
	getchar();
}