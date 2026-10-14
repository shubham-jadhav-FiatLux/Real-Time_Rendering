// Program For 'do-while' Loop Using Single Iterating Variable (Incrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	printf("Printing Digits From 1 To 10 : \n\n");

	shubh_i = 1;

	do {
	
		printf("\t%d\n", shubh_i);
		shubh_i++;

	}while (shubh_i <= 10);

	printf("\n\n");

	return 0;
}
