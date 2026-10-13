// Program To Calculate Sum Of Array Elements

#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void) {

	// Variable Declarations
	int shubh_iArray[NUM_ELEMENTS];
	int shubh_i, shubh_num, shubh_sum = 0;

	// Code
	printf("\n\n");
	printf("Enter Integer Elements For Array : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		scanf("%d", &shubh_num);
		shubh_iArray[shubh_i] = shubh_num;
	}

	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		shubh_sum += shubh_iArray[shubh_i];
	}

	printf("\n\n");
	printf("Sum Of All Elements Of Array = %d\n\n", shubh_sum);

	return 0;
}
