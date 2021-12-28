// Program For 2D Array Initialization And Displayed Using Nested Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} };		// Inline Initialization
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

	// *** Array Indices Begin From 0, Hence, 1st Row Is Actually 0th Row And 1st Column Is Actually 0th Column ***

	for (shubh_i = 0; shubh_i < shubh_iArray_num_rows; shubh_i++) {
	
		printf("****** ROW %d ******\n", (shubh_i + 1));
		for (shubh_j = 0; shubh_j < shubh_iArray_num_columns; shubh_j++) {
		
			printf("shubh_iArray[%d][%d] = %d\n", shubh_i, shubh_j, shubh_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	return 0;
}
