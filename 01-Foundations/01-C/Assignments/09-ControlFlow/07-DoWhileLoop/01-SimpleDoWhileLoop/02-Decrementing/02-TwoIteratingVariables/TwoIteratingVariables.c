// Program For 'do-while' Loop Using Two Iterating Variables (Decementing)

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	printf("Printing Digits 10 To 1 And 100 To 10 : \n\n");

	shubh_i = 10;
	shubh_j = 100;

	do {
	
		printf("\t%3d\t%3d\n", shubh_i, shubh_j);
		shubh_i--;
		shubh_j -= 10;
	} while (shubh_i >= 1, shubh_j >= 10);

	printf("\n\n");

	return 0;
}
