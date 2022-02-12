#include <stdio.h>
#include <stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_i, shubh_j;
	int** ptr_iArray = NULL;

	// Code
	printf("\n\n");
	ptr_iArray = (int**)malloc(NUM_ROWS * sizeof(int *));
	if (ptr_iArray == NULL) {
	
		printf("Memory allocation to the 1D array of base addresses of %d rows failed !! .Exiting ...\n\n", NUM_ROWS);
		exit(0);
	}
	else
		printf("Memory allocation to the 1D array of base addresses of %d rows has succeeded !!\n\n", NUM_ROWS);

	// Allocating memory to each row
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		ptr_iArray[shubh_i] = (int*)malloc(NUM_COLUMNS * sizeof(int));
		if (ptr_iArray == NULL) {

			printf("Memory allocation to the 1D array of base addresses of %d rows failed !! .Exiting ...\n\n", shubh_i);
			exit(0);
		}
		else
			printf("Memory allocation to the 1D array of base addresses of %d rows has succeeded !!\n\n", shubh_i);
	}

	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			*(*(ptr_iArray + shubh_i) + shubh_j) = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	printf("\n\n");
	printf("2D Integer Array Elements Along With Addresses : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			printf("ptr_iArray_Row[%d][%d] = %d \t\t At Address &ptr_iArray_Row[%d][%d] : %p\n", shubh_i, shubh_j, ptr_iArray[shubh_i][shubh_j],shubh_i, shubh_j, &ptr_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	// Freeing memory of each row
	for (shubh_i = (NUM_ROWS - 1); shubh_i >= 0; shubh_i--) {
	
		if (*(ptr_iArray + shubh_i)) {
		
			free(*(ptr_iArray + shubh_i));
			*(ptr_iArray + shubh_i) = NULL;
			printf("Memory allocated to row %d has been successfully freed !!\n\n", shubh_i);
		}
	}

	if (ptr_iArray) {

		free(ptr_iArray);
		ptr_iArray = NULL;
		printf("Memory allocated to ptr_iArray has been successfully freed !!\n\n");
	}

	return 0;
}
