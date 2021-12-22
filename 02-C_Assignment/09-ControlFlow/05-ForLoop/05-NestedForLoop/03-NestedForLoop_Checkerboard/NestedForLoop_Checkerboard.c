// Program For Nested for Loop To Implement Checkerboard

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j, shubh_c;

	// Code
	printf("\n\n");

	for (shubh_i = 0; shubh_i < 64; shubh_i++) {
	
		for (shubh_j = 0; shubh_j < 64; shubh_j++) {
		
			shubh_c = ((shubh_i & 0x8) == 0) ^ ((shubh_j & 0x8) == 0);

			if (shubh_c == 0)
				printf(" ");

			if (shubh_c == 1)
				printf("* ");
		}
		printf("\n\n");
	}

	return 0;
}
