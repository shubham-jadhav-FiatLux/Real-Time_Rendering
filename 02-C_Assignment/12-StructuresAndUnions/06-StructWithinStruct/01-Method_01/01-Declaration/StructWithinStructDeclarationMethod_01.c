#include <stdio.h>

struct Rectangle {

	struct MyPoint {

		int x;
		int y;
	} point_01, point_02;
} rect;

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_length, shubh_breadth, shubh_area;

	// Code
	printf("\n\n");
	printf("Enter leftmost x-coordinate of rectangle : ");
	scanf("%d", &rect.point_01.x);

	printf("\n\n");
	printf("Enter bottommost y-coordinate of rectangle : ");
	scanf("%d", &rect.point_01.y);

	printf("\n\n");
	printf("Enter rightmost x-coordinate of rectangle : ");
	scanf("%d", &rect.point_02.x);

	printf("\n\n");
	printf("Enter topmost y-coordinate of rectangle : ");
	scanf("%d", &rect.point_02.y);

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
