#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[5][3][2] = { { {9,8}, {27,36}, {45,54} },
								  { {8,16}, {24,32}, {40,48} },
								  { {7,14}, {21,28}, {35,42} },
								  { {6,12}, {18,24}, {30,36} },
								  { {5,10}, {15,20}, {25,30} }  };		// Inline Initialization
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements, shubh_iArray_width, shubh_iArray_height, shubh_iArray_depth;

	// Code
	printf("\n\n");

	shubh_int_size = sizeof(int);

	shubh_iArray_size = sizeof(shubh_iArray);
	printf("Size Of Three Dimentional (3D) Integer Array Is = %d\n\n", shubh_iArray_size);

	shubh_iArray_width = shubh_iArray_size / sizeof(shubh_iArray[0]);
	printf("Number Of Rows (width) In Three Dimentional (3D) Integer Array Is = %d\n\n", shubh_iArray_width);

	shubh_iArray_height = sizeof(shubh_iArray[0]) / sizeof(shubh_iArray[0][0]);
	printf("Number Of Columns In Three Dimentional (3D) Integer Array Is = %d\n\n", shubh_iArray_height);

	shubh_iArray_depth = sizeof(shubh_iArray[0][0]) / shubh_int_size;
	printf("Number Of Columns In Three Dimentional (3D) Integer Array Is = %d\n\n", shubh_iArray_height);

	shubh_iArray_num_elements = shubh_iArray_width * shubh_iArray_height * shubh_iArray_depth;
	printf("Number Of Elements In Three Dimentional (3D) Integer Array Is = %d\n\n", shubh_iArray_num_elements);

	printf("\n\n");
	printf("Elements In The 3D Array : \n\n");

	// *** ROW 1 ***
	printf("******* ROW 1 *******\n");
	printf("****** COLUMN 1 *******\n");
	printf("shubh_iArray[0][0][0] = %d\n", shubh_iArray[0][0][0]);		
	printf("shubh_iArray[0][0][1] = %d\n", shubh_iArray[0][0][1]);		
	printf("\n\n");

	printf("****** COLUMN 2 *******\n");
	printf("shubh_iArray[0][1][0] = %d\n", shubh_iArray[0][1][0]);
	printf("shubh_iArray[0][1][1] = %d\n", shubh_iArray[0][1][1]);
	printf("\n\n");

	printf("****** COLUMN 3 *******\n");
	printf("shubh_iArray[0][2][0] = %d\n", shubh_iArray[0][2][0]);
	printf("shubh_iArray[0][2][1] = %d\n", shubh_iArray[0][2][1]);
	printf("\n\n");


	// *** ROW 2 ***
	printf("******* ROW 2 *******\n");
	printf("****** COLUMN 1 *******\n");
	printf("shubh_iArray[1][0][0] = %d\n", shubh_iArray[1][0][0]);
	printf("shubh_iArray[1][0][1] = %d\n", shubh_iArray[1][0][1]);
	printf("\n\n");

	printf("****** COLUMN 2 *******\n");
	printf("shubh_iArray[1][1][0] = %d\n", shubh_iArray[1][1][0]);
	printf("shubh_iArray[1][1][1] = %d\n", shubh_iArray[1][1][1]);
	printf("\n\n");

	printf("****** COLUMN 3 *******\n");
	printf("shubh_iArray[1][2][0] = %d\n", shubh_iArray[1][2][0]);
	printf("shubh_iArray[1][2][1] = %d\n", shubh_iArray[1][2][1]);
	printf("\n\n");


	// *** ROW 3 ***
	printf("******* ROW 3 *******\n");
	printf("****** COLUMN 1 *******\n");
	printf("shubh_iArray[2][0][0] = %d\n", shubh_iArray[2][0][0]);
	printf("shubh_iArray[2][0][1] = %d\n", shubh_iArray[2][0][1]);
	printf("\n\n");

	printf("****** COLUMN 2 *******\n");
	printf("shubh_iArray[2][1][0] = %d\n", shubh_iArray[2][1][0]);
	printf("shubh_iArray[2][1][1] = %d\n", shubh_iArray[2][1][1]);
	printf("\n\n");

	printf("****** COLUMN 3 *******\n");
	printf("shubh_iArray[2][2][0] = %d\n", shubh_iArray[2][2][0]);
	printf("shubh_iArray[2][2][1] = %d\n", shubh_iArray[2][2][1]);
	printf("\n\n");


	// *** ROW 4 ***
	printf("******* ROW 4 *******\n");
	printf("****** COLUMN 1 *******\n");
	printf("shubh_iArray[3][0][0] = %d\n", shubh_iArray[3][0][0]);
	printf("shubh_iArray[3][0][1] = %d\n", shubh_iArray[3][0][1]);
	printf("\n\n");

	printf("****** COLUMN 2 *******\n");
	printf("shubh_iArray[3][1][0] = %d\n", shubh_iArray[3][1][0]);
	printf("shubh_iArray[3][1][1] = %d\n", shubh_iArray[3][1][1]);
	printf("\n\n");

	printf("****** COLUMN 3 *******\n");
	printf("shubh_iArray[3][2][0] = %d\n", shubh_iArray[3][2][0]);
	printf("shubh_iArray[3][2][1] = %d\n", shubh_iArray[3][2][1]);
	printf("\n\n");


	// *** ROW 5 ***
	printf("******* ROW 1 *******\n");
	printf("****** COLUMN 5 *******\n");
	printf("shubh_iArray[4][0][0] = %d\n", shubh_iArray[4][0][0]);
	printf("shubh_iArray[4][0][1] = %d\n", shubh_iArray[4][0][1]);
	printf("\n\n");

	printf("****** COLUMN 2 *******\n");
	printf("shubh_iArray[4][1][0] = %d\n", shubh_iArray[4][1][0]);
	printf("shubh_iArray[4][1][1] = %d\n", shubh_iArray[4][1][1]);
	printf("\n\n");

	printf("****** COLUMN 3 *******\n");
	printf("shubh_iArray[4][2][0] = %d\n", shubh_iArray[4][2][0]);
	printf("shubh_iArray[4][2][1] = %d\n", shubh_iArray[4][2][1]);
	printf("\n\n");

	return 0;
}
