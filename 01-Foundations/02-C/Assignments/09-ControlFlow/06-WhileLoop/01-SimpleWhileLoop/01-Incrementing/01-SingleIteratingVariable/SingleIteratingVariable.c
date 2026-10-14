// Program For 'while' Loop Using Single Iterating Variable (Incrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	printf("Printing Digits From 1 To 10 : \n\n");

	/*
	for (shubh_i = 1; shubh_i <= 10; shubh_i++) {
	
		printf("\t%d\n", shubh_i);
	}
	*/

	shubh_i = 1;

	while (shubh_i <= 10) {
	
		printf("\t%d\n", shubh_i);
		shubh_i++;
	}

	printf("\n\n");

	return 0;
}
