// Program For 'while' Loop Using Two Iterating Variables (Decrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	printf("Printing Numbers From 10 to 1 And 100 To 10 : \n\n");

	/*
	for (shubh_i = 10, shubh_j = 100; shubh_i >= 1, shubh_j >= 10; shubh_i--, shubh_j -= 10){
	
		printf("\t%d\t%d\n", shubh_i, shubh_j);
	}
	*/

	shubh_i = 10;
	shubh_j = 100;

	while (shubh_i >= 1, shubh_j >= 10) {
	
		printf("\t%3d\t%3d\n", shubh_i, shubh_j);
		shubh_i--;
		shubh_j -= 10;
	}

	printf("\n\n");

	return 0;
}
