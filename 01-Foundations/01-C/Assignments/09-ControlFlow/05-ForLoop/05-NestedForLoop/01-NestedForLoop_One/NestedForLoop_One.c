// program For Nested For Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	for (shubh_i = 1; shubh_i <= 10; shubh_i++) {
	
		printf("shubh_i = %d\n", shubh_i);
		printf("--------------------\n\n");

		for (shubh_j = 1; shubh_j <= 5; shubh_j++) {
		
			printf("\tshubh_j = %d\n", shubh_j);
		}
		printf("\n\n");
	}

	return 0;
}
