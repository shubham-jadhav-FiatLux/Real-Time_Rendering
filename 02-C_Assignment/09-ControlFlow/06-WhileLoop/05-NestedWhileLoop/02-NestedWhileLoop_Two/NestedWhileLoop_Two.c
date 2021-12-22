// Program For Nested While Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j, shubh_k;

	// Code
	printf("\n\n");

	shubh_i = 1;

	while (shubh_i <= 10) {
	
		printf("shubh_i = %d\n", shubh_i);
		printf("-----------------------\n\n");

		shubh_j = 1;

		while (shubh_j <= 5) {
		
			printf("\tshubh_j = %d\n", shubh_j);
			printf("\t--------------------------\n\n");

			shubh_k = 1;

			while (shubh_k <= 3) {
			
				printf("\t\tshubh_k = %d\n", shubh_k);
				printf("\t\t----------------------------\n\n");
				shubh_k++;
			}
			printf("\n\n");
			shubh_j++;
		}
		printf("\n\n");
		shubh_i++;
	}

	return 0;
}
