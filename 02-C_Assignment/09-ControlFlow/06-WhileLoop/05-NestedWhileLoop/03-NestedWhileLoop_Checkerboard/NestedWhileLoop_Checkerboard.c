// Program For Nested While Loop To Implement Checkerboard

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j, shubh_c;

	// Code
	printf("\n\n");

	shubh_i = 0;

	while (shubh_i < 64) {
	
		shubh_j = 0;

		while (shubh_j < 64) {
		
			shubh_c = ((shubh_i & 0x8) == 0) ^ ((shubh_j & 0x8) == 0);

			if (shubh_c == 0)
				printf(" ");

			if (shubh_c == 1)
				printf("* ");

			shubh_j++;
		}
		printf("\n\n");
		shubh_i++;
	}

	return 0;
}
