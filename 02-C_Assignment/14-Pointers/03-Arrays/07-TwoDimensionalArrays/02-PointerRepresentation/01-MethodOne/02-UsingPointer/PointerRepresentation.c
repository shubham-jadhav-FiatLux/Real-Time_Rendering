#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray[NUM_ROWS][NUM_COLUMNS];
	int shubh_i, shubh_j;

	int* ptr_iArray_Row = NULL;

	// Code
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		ptr_iArray_Row = shubh_iArray[shubh_i];
		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			*(ptr_iArray_Row + shubh_j) = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	printf("\n\n");
	printf("2D Integer Array Elements Along With Addresses : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ROWS; shubh_i++) {

		ptr_iArray_Row = shubh_iArray[shubh_i];
		for (shubh_j = 0; shubh_j < NUM_COLUMNS; shubh_j++) {

			printf("*(ptr_iArray_Row + %d) = %d \t\t At Address (ptr_iArray_Row + j) : %p\n", shubh_j, *(ptr_iArray_Row + shubh_j), (ptr_iArray_Row + shubh_j));
		}
		printf("\n\n");
	}

	return 0;
}
