#include <stdio.h>

// Defining Struct
struct MyPoint {

	int x;
	int y;
};

int main(int argc, char* argv[], char* envp[]) {

	struct MyPoint point_A, point_B, point_C, point_D, point_E;		// Defclaring 5 struct variables of type 'struct MyPoint' globally ...

	// Code
	// Assigning data values to the data members of 'struct MyPoint' variable 'point_A'
	point_A.x = 3;
	point_A.y = 0;

	// Assigning data values to the data members of 'struct MyPoint' variable 'point_C'
	point_B.x = 9;
	point_B.y = 6;

	// Assigning data values to the data members of 'struct MyPoint' variable 'point_D'
	point_C.x = 8;
	point_C.y = 2;

	// Assigning data values to the data members of 'struct MyPoint' variable 'point_E'
	point_D.x = 11;
	point_D.y = 8;

	// Assigning data values to the data members of 'struct MyPoint' variable 'point_A'
	point_E.x = 1;
	point_E.y = 2;

	// Displaying values of the data members of 'struct MyPoint' (all variables)
	printf("\n\n");
	printf("Co-ordinates (x, y) of point 'A' are : (%d, %d)\n\n", point_A.x, point_A.y);
	printf("Co-ordinates (x, y) of point 'B' are : (%d, %d)\n\n", point_B.x, point_B.y);
	printf("Co-ordinates (x, y) of point 'C' are : (%d, %d)\n\n", point_C.x, point_C.y);
	printf("Co-ordinates (x, y) of point 'D' are : (%d, %d)\n\n", point_D.x, point_D.y);
	printf("Co-ordinates (x, y) of point 'E' are : (%d, %d)\n\n", point_E.x, point_E.y);

	return 0;
}
