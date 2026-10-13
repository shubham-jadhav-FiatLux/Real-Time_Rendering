// Program For Nested Do While Loop

#include <stdio.h>

int main(void) {

	// Variable Declaratons
	int shubh_i, shubh_j, shubh_k;

	// Code
	printf("\n\n");

	shubh_i = 1;

	do {
	
		printf("shubh_i = %d\n", shubh_i);
		printf("------------------------\n\n");

		shubh_j = 1;

		do {
		
			printf("\tshubh_j = %d\n", shubh_j);
			printf("\t------------------------\n\n");

			shubh_k = 1;

			do {
			
				printf("\t\tshubh_k = %d\n", shubh_k);
				printf("\t\t---------------------------\n\n");
				shubh_k++;
			} while (shubh_k <= 3);

			printf("\n\n");
			shubh_j++;
		} while (shubh_j <= 5);

		printf("\n\n");
		shubh_i++;
	} while (shubh_i <= 10);

	return 0;
}