// Program For 2D Array piece-meal Initialization

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[3][5];		// 3 Rows And 5 Columns
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements, shubh_iArray_num_rows, shubh_iArray_num_columns;
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	shubh_int_size = sizeof(int);

	shubh_iArray_size = sizeof(shubh_iArray);
	printf("Size Of Two Dimentional (2D) Integer Array Is = %d\n\n", shubh_iArray_size);

	shubh_iArray_num_rows = shubh_iArray_size / sizeof(shubh_iArray[0]);
	printf("Number Of Rows In Two Dimentional (2D) Integer Array Is = %d\n\n", shubh_iArray_num_rows);

	shubh_iArray_num_columns = sizeof(shubh_iArray[0]) / shubh_int_size;
	printf("Number Of Columns In Two Dimentional (2D) Integer Array Is = %d\n\n", shubh_iArray_num_columns);

	shubh_iArray_num_elements = shubh_iArray_num_rows * shubh_iArray_num_columns;
	printf("Number Of Elements In Two Dimentional (2D) Integer Array Is = %d\n\n", shubh_iArray_num_elements);

	printf("\n\n");
	printf("Elements In The 2D Array : \n\n");

	// *** PIECE-MEAL ASSIGNMENT ***
	// ******* ROW 1 *******
	shubh_iArray[0][0] = 21;
	shubh_iArray[0][1] = 42;
	shubh_iArray[0][2] = 63;
	shubh_iArray[0][3] = 84;
	shubh_iArray[0][4] = 105;

	// ******* ROW 2 *******
	shubh_iArray[1][0] = 22;
	shubh_iArray[1][1] = 44;
	shubh_iArray[1][2] = 66;
	shubh_iArray[1][3] = 88;
	shubh_iArray[1][4] = 110;

	// ******* ROW 3 *******
	shubh_iArray[2][0] = 23;
	shubh_iArray[2][1] = 46;
	shubh_iArray[2][2] = 69;
	shubh_iArray[2][3] = 92;
	shubh_iArray[2][4] = 115;

	// **** DISPLAY ****
	for (shubh_i = 0; shubh_i < shubh_iArray_num_rows; shubh_i++) {
	
		printf("**** ROW %d ****\n", (shubh_i + 1));
		for (shubh_j = 0; shubh_j < shubh_iArray_num_columns; shubh_j++) {
		
			printf("shubh_iArray[%d][%d] = %d\n", shubh_i, shubh_j, shubh_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	return 0;
}
