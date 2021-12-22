// Program For 'do-while' Loop Using Two Iterating Variables (Incrementing)

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	printf("Printing Numbers From 1 To 10 And 1 To 100 : \n\n");

	shubh_i = 1;
	shubh_j = 10;

	do {
	
		printf("\t%3d\t%3d\n", shubh_i, shubh_j);
		shubh_i++;
		shubh_j += 10;
	} while (shubh_i <= 10, shubh_j <= 100);

	printf("\n\n");

	return 0;
}
