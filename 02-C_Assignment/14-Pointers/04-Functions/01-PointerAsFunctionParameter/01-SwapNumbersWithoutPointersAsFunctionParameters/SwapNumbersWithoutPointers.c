#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Declarations
	void SwapNumbers(int, int);

	// Variable Declarations
	int shubh_a;
	int shubh_b;

	// Code
	printf("\n\n");
	printf("Enter Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Value For 'B' : ");
	scanf("%d", &shubh_b);

	printf("\n\n");
	printf("**** Before Swapping *****\n\n");
	printf("Value of 'A' = %d\n\n", shubh_a);
	printf("Value of 'B' = %d\n\n", shubh_b);

	SwapNumbers(shubh_a, shubh_b);
	
	printf("\n\n");
	printf("**** After Swapping *****\n\n");
	printf("Value of 'A' = %d\n\n", shubh_a);
	printf("Value of 'B' = %d\n\n", shubh_b);

	return 0;
}

void SwapNumbers(int x, int y) {

	// Variable Declarations
	int temp;

	// Code
	printf("\n\n");

	printf("\n\n");
	printf("**** Before Swapping *****\n\n");
	printf("Value of 'X' = %d\n\n", x);
	printf("Value of 'Y' = %d\n\n", y);

	temp = x;
	x = y;
	y = temp;

	printf("\n\n");
	printf("**** After Swapping *****\n\n");
	printf("Value of 'X' = %d\n\n", x);
	printf("Value of 'Y' = %d\n\n", y);

}
