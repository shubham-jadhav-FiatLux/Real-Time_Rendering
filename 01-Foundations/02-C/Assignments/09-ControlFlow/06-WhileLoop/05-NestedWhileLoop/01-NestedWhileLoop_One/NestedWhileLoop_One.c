// Program For Nested While Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	shubh_i = 1;

	while (shubh_i <= 10) {
	
		printf("shubh_i = %d\n", shubh_i);
		printf("------------------------\n\n");

		shubh_j = 1;

		while (shubh_j <= 5) {
		
			printf("\tshubh_j = %d\n", shubh_j);
			shubh_j++;
		}

		shubh_i++;
		printf("\n\n");
	}

	return 0;
}