#include <stdio.h>

#include "world_transforms_tests.c"
#include "math_tests.c"

int main()
{
	RunWorldTransformsTests();

	RunGameTests();

	printf("Press any key to continue...");
	getchar();
}