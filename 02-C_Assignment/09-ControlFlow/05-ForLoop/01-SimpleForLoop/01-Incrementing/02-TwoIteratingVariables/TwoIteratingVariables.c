// Program For 'for' Using Two Iterating Variables

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	printf("Printing Numbers From 1 To 10 And 10 To 100 : \n\n");

	for (shubh_i = 1, shubh_j = 10; shubh_i <= 10, shubh_j <= 100; shubh_i++, shubh_j += 10) {
	
		printf("\t%3d\t%3d\n", shubh_i, shubh_j);
	}

	printf("\n\n");

	return 0;
}


// BEHAIVIOUR OF 'COMMA' OPERATOR
/*
	As we can see , in for loop condition right side condition dominates over left side condition.
	This happens because comma operator has least priority among all operators and
	it has left-to-right associativity.
*/