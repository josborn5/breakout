#include <stdio.h>

#include "world_transforms_tests.c"

static int main()
{
	RunWorldTransformsTests();

	printf("Press any key to continue...");
	getchar();
}