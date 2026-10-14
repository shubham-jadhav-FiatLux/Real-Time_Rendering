// Program To Seperate Odd ANd Even Numbers From Array

#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void) {

	// Vriable Declarations
	int shubh_iArray[NUM_ELEMENTS];
	int shubh_i, shubh_num;

	// ***** ARRAY ELEMENTS INPUT ******
	printf("\n\n");
	printf("Enter Integer Elements For Array : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		scanf("%d", &shubh_num);
		shubh_iArray[shubh_i] = shubh_num;
	}

	// *** Seperating Out Even Numbers From Array Elements ***
	printf("\n\n");
	printf("Even Numbers Among Array Elements Are : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		if (shubh_iArray[shubh_i] % 2 == 0)
			printf("%d\n", shubh_iArray[shubh_i]);
	}


	// *** Seperating Out Odd Numbers From Array Elements ***
	printf("\n\n");
	printf("Even Numbers Among Array Elements Are : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {

		if (shubh_iArray[shubh_i] % 2 != 0)
			printf("%d\n", shubh_iArray[shubh_i]);
	}

	return 0;
}
