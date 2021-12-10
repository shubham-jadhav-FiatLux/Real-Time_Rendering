// Program For If Statement

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int a;

	// Code
	printf("\n\n");

	a = 5;
	if (a) {	//Non-Zero Positive value

		printf("If-Block 1 : 'A' Exists And Has Value = %d !!\n\n", a);
	}

	a = -5;
	if (a) {	//Non-Zero Negative value
	
		printf("If-Block 2 : 'A' Exists And Has Value = %d !!\n\n", a);
	}

	a = 0;
	if (a) {	//Zero Value
	
		printf("If-Block 3 : 'A' Exists And Has Value = %d !!\n\n", a);
	}

	printf("All Three If-Statements Are Done !!\n\n");

	return 0;
}
