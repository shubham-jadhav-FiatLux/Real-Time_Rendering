#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray2D[NUM_ROWS][NUM_COLUMNS];
	int shubh_iArray1D[NUM_ROWS * NUM_COLUMNS];
	int shubh_i, shubh_j;
	int shubh_num;

	// Code
	printf("Enter elements of yuor choice to fill up the Integer 2D array : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {
	
		printf("For ROW NUMBER %d : \n", (shubh_i+1));
		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {
		
			printf("Enter element number %d : \n", (shubh_j+1));
			scanf("%d", &shubh_num);
			shubh_iArray2D[shubh_i][shubh_j] = shubh_num;
		}
		printf("\n\n");
	}

	// *** Display 2D Array ***
	printf("\n\n");
	printf("Two-Dimensional (2D) array of integers :\n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		printf("**** ROW %d ****: \n", (shubh_i + 1));
		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			printf("shubh_iArray2D[%d][%d] = %d\n", shubh_i, shubh_j, shubh_iArray2D[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	// Converting 2D integer array to 1D integer array
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			shubh_iArray1D[shubh_i * NUM_COLUMNS + shubh_j] = shubh_iArray2D[shubh_i][shubh_j];
		}
		printf("\n");
	}

	// Printing 1D array
	printf("One-Dimensional (1D) array of integers :\n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS * NUM_COLUMNS; shubh_i++) {

			printf("shubh_iArray1D[%d] = %d\n", shubh_i, shubh_iArray1D[shubh_i]);
	}
	printf("\n\n");

	return 0;
}
