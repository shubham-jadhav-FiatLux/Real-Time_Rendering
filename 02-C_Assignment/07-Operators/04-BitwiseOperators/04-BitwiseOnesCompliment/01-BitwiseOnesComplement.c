// Program For Bitwise Ones Complement

#include <stdio.h>

int main(void) {

	// Function Prototypes
	void PrintBinaryFormOfNumber(unsigned int);

	// Variable Declarations
	unsigned int a;
	unsigned int result;

	// Code
	printf("\n\n");
	printf("Enter An Integer : ");
	scanf("%u", &a);

	printf("\n\n\n");
	result = ~a;
	printf("Bitwise COMPLEMENTING Of \na = %d (Decimal) Gives Result %d (Decimal).\n\n", a, result);

	PrintBinaryFormOfNumber(a);
	PrintBinaryFormOfNumber(result);

	return 0;
}

void PrintBinaryFormOfNumber(unsigned int decimal_number) {

	// Variable Declarations
	unsigned int quotient, remainder;
	unsigned int num;
	unsigned int binary_array[8];
	int i;

	// Code
	for (i = 0; i < 8; i++)
		binary_array[i] = 0;

	printf("The Binary Form Of The Decimal Integer %d Is       \t = \t", decimal_number);
	num = decimal_number;
	i = 7;
	while (num != 0) {

		quotient = num / 2;
		remainder = num % 2;
		binary_array[i] = remainder;
		num = quotient;
		i--;
	}

	for (i = 0; i < 8; i++)
		printf("%u", binary_array[i]);

	printf("\n\n");
}
