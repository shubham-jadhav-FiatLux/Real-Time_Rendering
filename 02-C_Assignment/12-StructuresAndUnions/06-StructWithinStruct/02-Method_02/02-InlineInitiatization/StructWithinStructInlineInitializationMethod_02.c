#include <stdio.h>

struct MyPoint {

	int x;
	int y;
};

struct Rectangle {

	struct MyPoint point_01;
	struct MyPoint point_02;
};

struct Rectangle rect = { {2, 3}, {5, 6} };

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_length, shubh_breadth, shubh_area;

	// Code
	shubh_length = rect.point_02.y - rect.point_01.y;
	if (shubh_length < 0)
		shubh_length *= -1;

	shubh_breadth = rect.point_02.x - rect.point_01.x;
	if (shubh_breadth < 0)
		shubh_breadth *= -1;

	shubh_area = shubh_length * shubh_breadth;

	printf("\n\n");
	printf("Length of rectangle = %d\n\n", shubh_length);
	printf("Breadth of rectangle = %d\n\n", shubh_breadth);
	printf("Area of rectangle = %d\n\n", shubh_area);

	return 0;
}
