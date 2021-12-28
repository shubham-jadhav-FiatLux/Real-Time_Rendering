// Program For 2D Array Initialization

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} };		// Inline Initialization
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements, shubh_iArray_num_rows, shubh_iArray_num_columns;

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

	// *** ROW 1 ***
	printf("******* ROW 1 *******\n");
	printf("shubh_iArray[0][0] = %d\n", shubh_iArray[0][0]);		// *** COLUMN 1 *** (0th Element)
	printf("shubh_iArray[0][1] = %d\n", shubh_iArray[0][1]);		// *** COLUMN 2 *** (1th Element)
	printf("shubh_iArray[0][2] = %d\n", shubh_iArray[0][2]);		// *** COLUMN 3 *** (2th Element)
	printf("\n\n");

	// *** ROW 2 ***
	printf("******* ROW 2 *******\n");
	printf("shubh_iArray[1][0] = %d\n", shubh_iArray[1][0]);		// *** COLUMN 1 *** (0th Element)
	printf("shubh_iArray[1][1] = %d\n", shubh_iArray[1][1]);		// *** COLUMN 2 *** (1th Element)
	printf("shubh_iArray[1][2] = %d\n", shubh_iArray[1][2]);		// *** COLUMN 3 *** (2th Element)
	printf("\n\n");

	// *** ROW 3 ***
	printf("******* ROW 3 *******\n");
	printf("shubh_iArray[2][0] = %d\n", shubh_iArray[2][0]);		// *** COLUMN 1 *** (0th Element)
	printf("shubh_iArray[2][1] = %d\n", shubh_iArray[2][1]);		// *** COLUMN 2 *** (1th Element)
	printf("shubh_iArray[2][2] = %d\n", shubh_iArray[2][2]);		// *** COLUMN 3 *** (2th Element)
	printf("\n\n");

	// *** ROW 4 ***
	printf("******* ROW 4 *******\n");
	printf("shubh_iArray[3][0] = %d\n", shubh_iArray[3][0]);		// *** COLUMN 1 *** (0th Element)
	printf("shubh_iArray[3][1] = %d\n", shubh_iArray[3][1]);		// *** COLUMN 2 *** (1th Element)
	printf("shubh_iArray[3][2] = %d\n", shubh_iArray[3][2]);		// *** COLUMN 3 *** (2th Element)
	printf("\n\n");

	// *** ROW 5 ***
	printf("******* ROW 5 *******\n");
	printf("shubh_iArray[4][0] = %d\n", shubh_iArray[4][0]);		// *** COLUMN 1 *** (0th Element)
	printf("shubh_iArray[4][1] = %d\n", shubh_iArray[4][1]);		// *** COLUMN 2 *** (1th Element)
	printf("shubh_iArray[4][2] = %d\n", shubh_iArray[4][2]);		// *** COLUMN 3 *** (2th Element)
	printf("\n\n");

	return 0;
}
