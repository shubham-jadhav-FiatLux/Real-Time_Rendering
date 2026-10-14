// Program For Use Of 'continue'

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	printf("Printing Even Numbers From 0 To 100 : \n\n");

	for (shubh_i = 0; shubh_i <= 100; shubh_i++) {
	
		//Condition For A Number To Be Even Number => Division Of Number By 2 Leaves NO Remainder (remainder = 0)
		//If Remainder Is NOT 0, The Number Is Odd Number...

		if (shubh_i % 2 != 0) {
		
			continue;
		}
		else {
		
			printf("\t%d\n", shubh_i);
		}
	}

	printf("\n\n");

	return 0;
}
