// Program For 'do-while' Loop Using Single Iterating Variable (Dcrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	printf("Printing Digits From 10 To 1 : \n\n");

	shubh_i = 10;

	do {
	
		printf("\t%d\n", shubh_i);
		shubh_i--;
	} while (shubh_i >= 1);

	printf("\n\n");

	return 0;
}
