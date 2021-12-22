// Program For 'while' Loop Using Single Iterating Variable (Decrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	printf("Printing Digits From 10 To 1 : \n\n");

	/*
	for (shubh_i = 10; shubh_i >= 1; shubh_i--){
	
		printf("\t%d\n", shubh_i);
	}
	*/ 

	shubh_i = 10;

	while (shubh_i >= 1) {
	
		printf("\t%d\n", shubh_i);
		shubh_i--;
	}

	printf("\n\n");

	return 0;
}
