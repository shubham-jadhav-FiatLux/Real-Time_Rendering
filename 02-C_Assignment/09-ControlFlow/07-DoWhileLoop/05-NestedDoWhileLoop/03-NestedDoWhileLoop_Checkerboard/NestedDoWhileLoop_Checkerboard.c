// Program For Nested Do While Loop To Implement Checkerboard

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j, shubh_c;

	// Code
	printf("\n\n");

	shubh_i = 0;

	do {
	
		shubh_j = 0;

		do {
		
			shubh_c = ((shubh_i & 0x8) == 0) ^ ((shubh_j & 0x8) == 0);

			if (shubh_c == 0)
				printf(" ");

			if (shubh_c == 1)
				printf("* ");

			shubh_j++;
		} while (shubh_j < 64);

		printf("\n\n");
		shubh_i++;
	} while (shubh_i < 64);

	return 0;
}
