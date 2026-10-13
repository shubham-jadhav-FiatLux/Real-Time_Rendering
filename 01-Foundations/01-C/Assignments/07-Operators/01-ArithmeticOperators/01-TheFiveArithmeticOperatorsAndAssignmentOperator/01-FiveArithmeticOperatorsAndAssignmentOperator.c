// Program For Arithmetic And Assignment Operators

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int a;
	int b;
	int result;

	// Code
	printf("\n\n");
	printf("Enter A Number : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &b);

	//*** The Following Are The 5 Arithmetic Operators +, -, *, /, % ***
	//*** Also, The Resultants Of Arithmetic Operations In All The Below Five Cases Have Been Assigned To The Variable 'result' Using The Assignment Operator '=' ***

	printf("\n\n");

	result = a + b;
	printf("Addition Of a = %d And b = %d Gives %d\n", a, b, result);

	result = a - b;
	printf("Substraction Of a = %d And b = %d Gives %d\n", a, b, result);

	result = a * b;
	printf("Multiplication Of a = %d And b = %d Gives %d\n", a, b, result);

	result = a / b;
	printf("Division of a = %d And b = %d Gives Quotient %d\n", a, b, result);

	result = a % b;
	printf("Division Of a = %d And b = %d Gives Remainder %d\n", a, b, result);

	printf("\n\n");

	return 0;
}
