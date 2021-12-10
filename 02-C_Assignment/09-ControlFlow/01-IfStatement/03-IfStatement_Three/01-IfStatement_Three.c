// Program For If Statement

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int num;

	// Code
	printf("\n\n");

	printf("Enter Value For 'Num' : ");
	scanf("%d", &num);

	if (num < 0) {
	
		printf("Num = %d Is Less Than 0 (NEGATIVE).\n\n", num);
	}

	if ((num > 0) && (num <= 100)) {
	
		printf("Num = %d Is Between 0 and 100.\n\n", num);
	}

	if ((num > 100) && (num <= 200)) {
	
		printf("Num = %d Is Between 100 And 200.\n\n", num);
	}

	if ((num > 200) && (num <= 300)) {
	
		printf("Num = %d Is Between 200 And 300.\n\n", num);
	}

	if ((num > 300) && (num <= 400)) {
	
		printf("Num = %d Is Between 300 And 400.\n\n", num);
	}

	if ((num > 400) && (num <= 500)) {
	
		printf("Num = %d Is Between 400 And 500.\n\n", num);
	}

	if (num > 500) {
	
		printf("Num = %d Is Greater Than 500.\n\n", num);
	}

	return 0;
}
