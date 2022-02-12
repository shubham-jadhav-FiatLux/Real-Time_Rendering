#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS_ONE 3
#define NUM_COLUMNS_TWO 8

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int* shubh_iArray[NUM_ROWS];
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");
	printf("************ First memory allocation to 2D integer array **************\n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		shubh_iArray[shubh_i] = (int*)malloc(NUM_COLUMNS_ONE * sizeof(int));
		if (shubh_iArray[shubh_i] == NULL) {

			printf("Failed to allocate memory to row %d of 2D integer array!! .Exiting ...\n\n", shubh_i);
			exit(0);
		}
		else
			printf("Memory allocation to row %d of 2D integer array succeeded !!\n\n", shubh_i);
	}

	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS_ONE; shubh_j++) {

			shubh_iArray[shubh_i][shubh_j] = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	printf("\n\n");
	printf("Displaying 2D Array: \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS_ONE; shubh_j++) {

			printf("shubh_iArray[%d][%d] = %d\n", shubh_i, shubh_j, shubh_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	printf("\n\n");

	// Freeing memory of each row
	for (shubh_i = (NUM_ROWS - 1); shubh_i >= 0; shubh_i--) {

		if (shubh_iArray[shubh_i]) {

			free(shubh_iArray[shubh_i]);
			shubh_iArray[shubh_i] = NULL;
			printf("Memory allocated to row %d of 2D integer array has been successfully freed !!\n\n", shubh_i);
		}
	}


	printf("\n\n");
	printf("************ Second memory allocation to 2D integer array **************\n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		shubh_iArray[shubh_i] = (int*)malloc(NUM_COLUMNS_TWO * sizeof(int));
		if (shubh_iArray[shubh_i] == NULL) {

			printf("Failed to allocate memory to row %d of 2D integer array!! .Exiting ...\n\n", shubh_i);
			exit(0);
		}
		else
			printf("Memory allocation to row %d of 2D integer array succeeded !!\n\n", shubh_i);
	}

	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS_TWO; shubh_j++) {

			shubh_iArray[shubh_i][shubh_j] = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	printf("\n\n");
	printf("Displaying 2D Array: \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS_TWO; shubh_j++) {

			printf("shubh_iArray[%d][%d] = %d\n", shubh_i, shubh_j, shubh_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	printf("\n\n");

	// Freeing memory of each row
	for (shubh_i = (NUM_ROWS - 1); shubh_i >= 0; shubh_i--) {

		if (shubh_iArray[shubh_i]) {

			free(shubh_iArray[shubh_i]);
			shubh_iArray[shubh_i] = NULL;
			printf("Memory allocated to row %d of 2D integer array has been successfully freed !!\n\n", shubh_i);
		}
	}

	return 0;
}

