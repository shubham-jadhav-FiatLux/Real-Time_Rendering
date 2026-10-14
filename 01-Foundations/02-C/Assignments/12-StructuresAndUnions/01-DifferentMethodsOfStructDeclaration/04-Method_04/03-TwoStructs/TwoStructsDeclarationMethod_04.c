#include <stdio.h>

// Defining Struct
struct MyPoint {

	int x;
	int y;
};

// Defining Struct
struct MyPointProperties {

	int quadrant;
	char axis_location[10];
};

int main(int argc, char* argv[], char* envp[]) {

	struct MyPoint point;

	struct MyPointProperties point_properties;

	// Code
	// User input for the data members of 'struct MyPoint' variable 'point_A'
	printf("\n\n");
	printf("Enter x-coordinate for a point : ");
	scanf("%d", &point.x);
	printf("Enter y-coordinate for a point : ");
	scanf("%d", &point.y);

	printf("\n\n");
	printf("Point co-ordinates (x, y) are : (%d, %d) !!\n", point.x, point.y);

	if (point.x == 0 && point.y == 0)
		printf("The Point is the origin (%d, %d) !!\n", point.x, point.y);
	else {

		if (point.x == 0) {

			if (point.y < 0)
				strcpy(point_properties.axis_location, "Negative Y");

			if (point.y > 0)
				strcpy(point_properties.axis_location, "Positive Y");

			point_properties.quadrant = 0;
			printf("The point lies on the %s axis !!\n\n", point_properties.axis_location);
		}
		else if (point.y == 0) {

			if (point.x < 0)
				strcpy(point_properties.axis_location, "Negative Y");

			if (point.x > 0)
				strcpy(point_properties.axis_location, "Positive Y");

			point_properties.quadrant = 0;
			printf("The point lies on the %s axis !!\n\n", point_properties.axis_location);
		}
		else {

			point_properties.axis_location[0] = '\0';

			if (point.x > 0 && point.y > 0)
				point_properties.quadrant = 1;

			else if (point.x < 0 && point.y > 0)
				point_properties.quadrant = 2;

			else if (point.x < 0 && point.y < 0)
				point_properties.quadrant = 3;

			else
				point_properties.quadrant = 4;

			printf("The point lies in Quadrant number %d !!\n\n", point_properties.quadrant);
		}
	}
	return 0;
}
