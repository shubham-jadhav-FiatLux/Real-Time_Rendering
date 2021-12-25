// Return Value Of Function As Return Value Of Another Function

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	int MyAddition(int, int);

	// variable Declarations
	int shubh_num1, shubh_num2;

	// Code
	shubh_num1 = 10;
	shubh_num2 = 20;

	printf("\n\n");
	printf("%d + %d = %d\n\n", shubh_num1, shubh_num2, MyAddition(shubh_num1, shubh_num2));

	return 0;
}


int MyAddition(int shubh_one, int shubh_two) {

	// Function Prototype
	int Add(int, int);

	// Code
	return (Add(shubh_one, shubh_two));		// Return Value Of Function Add() As Return Value Of Function MyAddition()
}


int Add(int shubh_x, int shubh_y) {

	// Code
	return (shubh_x + shubh_y);
}
