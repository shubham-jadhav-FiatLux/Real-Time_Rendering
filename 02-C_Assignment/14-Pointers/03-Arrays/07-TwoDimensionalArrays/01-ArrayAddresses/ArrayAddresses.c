#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray[NUM_ROWS][NUM_COLUMNS];
	int shubh_i, shubh_j;

	// Code
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {
	
		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			shubh_iArray[shubh_i][shubh_j] = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	printf("\n\n");
	printf("2D Integer Array Elements Along With Addresses : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			printf("shubh_iArray[%d][%d] = %d \t\t At Address : %p\n", shubh_i, shubh_j, shubh_iArray[shubh_i][shubh_j], &shubh_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	return 0;
}
