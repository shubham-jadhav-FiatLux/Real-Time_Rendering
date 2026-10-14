// Program for relational operators

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int a;
	int b;
	int result;

	// Code
	printf("\n\n");
	printf("Enter One Integer : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &b);

	printf("\n\n");
	printf("If Answer = 0, It Is 'FALSE'\n");
	printf("If Answer = 1, It Is 'TRUE'\n\n");

	
	result = (a < b);
	printf("(a < b) a = %d Is Less Than b = %d                     \t Answer = %d\n", a, b, result);

	result = (a > b);
	printf("(a > b) a = %d Is Greater Than b = %d                  \t Answer = %d\n", a, b, result);

	result = (a <= b);
	printf("(a <= b) a = %d Is Less Than Or Equal To b = %d        \t Answer = %d\n", a, b, result);

	result = (a >= b);
	printf("(a >= b) a = %d Is Greater Than Or Equal To b = %d     \t Answer = %d\n", a, b, result);

	result = (a == b);
	printf("(a == b) a = %d Is Equal To b = %d                     \t Answer = %d\n", a, b, result);

	result = (a != b);
	printf("(a != b) a = %d Is NOT Equal To b = %d                 \t Answer = %d\n", a, b, result);

	return 0;
}
