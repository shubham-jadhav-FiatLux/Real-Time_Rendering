#include <stdio.h>

// Defining Struct
struct MyPoint {

	int x;
	int y;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declaration
	struct MyPoint point_A, point_B, point_C, point_D, point_E;		// Defclaring 5 struct variables of type 'struct MyPoint' globally ...

	// Code
	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter X-coordinate For Point 'A' : ");
	scanf("%d", &point_A.x);
	printf("Enter Y-coordinate For Point 'A' : ");
	scanf("%d", &point_A.y);

	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter X-coordinate For Point 'B' : ");
	scanf("%d", &point_B.x);
	printf("Enter Y-coordinate For Point 'B' : ");
	scanf("%d", &point_B.y);

	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter X-coordinate For Point 'C' : ");
	scanf("%d", &point_C.x);
	printf("Enter Y-coordinate For Point 'C' : ");
	scanf("%d", &point_C.y);

	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter X-coordinate For Point 'D' : ");
	scanf("%d", &point_D.x);
	printf("Enter Y-coordinate For Point 'D' : ");
	scanf("%d", &point_D.y);

	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter X-coordinate For Point 'E' : ");
	scanf("%d", &point_E.x);
	printf("Enter Y-coordinate For Point 'E' : ");
	scanf("%d", &point_E.y);


	// Displaying values of the data members of 'struct MyPoint' (all variables)
	printf("\n\n");
	printf("Co-ordinates (x, y) of point 'A' are : (%d, %d)\n\n", point_A.x, point_A.y);
	printf("Co-ordinates (x, y) of point 'B' are : (%d, %d)\n\n", point_B.x, point_B.y);
	printf("Co-ordinates (x, y) of point 'C' are : (%d, %d)\n\n", point_C.x, point_C.y);
	printf("Co-ordinates (x, y) of point 'D' are : (%d, %d)\n\n", point_D.x, point_D.y);
	printf("Co-ordinates (x, y) of point 'E' are : (%d, %d)\n\n", point_E.x, point_E.y);

	return 0;
}
