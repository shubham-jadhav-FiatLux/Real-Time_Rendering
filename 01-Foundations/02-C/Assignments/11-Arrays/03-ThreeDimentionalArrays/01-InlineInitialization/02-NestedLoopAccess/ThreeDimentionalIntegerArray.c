#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[5][3][2] = { { {9,8}, {27,36}, {45,54} },
								  { {8,16}, {24,32}, {40,48} },
								  { {7,14}, {21,28}, {35,42} },
								  { {6,12}, {18,24}, {30,36} },
								  { {5,10}, {15,20}, {25,30} } };		// Inline Initialization
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements, shubh_iArray_width, shubh_iArray_height, shubh_iArray_depth;
	int i, j, k;

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

	for (i = 0; i < shubh_iArray_width; i++) {

		printf("****** ROW %d ******\n", (i + 1));
		for (j = 0; j < shubh_iArray_height; j++) {

			printf("****** COLUMN %d ******\n", (j + 1));
			for (k = 0; k < shubh_iArray_depth; k++) {

				printf("iArray[%d][%d][%d] = %d\n", i, j, k, shubh_iArray[i][j][k]);
			}
			printf("\n");
		}
		printf("\n\n");
	}


	return 0;
}
