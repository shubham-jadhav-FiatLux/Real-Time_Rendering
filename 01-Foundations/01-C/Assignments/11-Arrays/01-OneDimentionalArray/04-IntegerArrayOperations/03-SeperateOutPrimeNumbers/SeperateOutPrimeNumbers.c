// Program  TO  Seperate Out Prime Numbers From Array

#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void) {

	// variable Declarations
	int shubh_iArray[NUM_ELEMENTS];
	int shubh_i, shubh_num, shubh_j, shubh_count = 0;

	// Code
	printf("\n\n");

	// **** ARRAY ELEMENTS INPUT ****
	printf("Enter Integer Elements For Array : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		scanf("%d", &shubh_num);
		if (shubh_num < 0)				// if num is negative (< 0), then conver it to positive (Multiply by -1)
			shubh_num = -1 * shubh_num;

		shubh_iArray[shubh_i] = shubh_num;
	}

	// ****	PRINTING ENTIRE ARRAY ****	
	printf("\n\n");
	printf("Array Elements Are : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		printf("%d\n", shubh_iArray[shubh_i]);
	}

	// *** Seperating Out Prime Numbers From array Elements ***
	printf("\n\n");
	printf("Prime Numbers Amongst The Array Elements Are : \n\n");
	for (shubh_i = 0; shubh_i < NUM_ELEMENTS; shubh_i++) {
	
		for (shubh_j = 1; shubh_j <= shubh_iArray[shubh_i]; shubh_j++) {
		
			if ((shubh_iArray[shubh_i] % shubh_j) == 0)
				shubh_count++;
		}
		
		//Number 1 Is Neither a Prime Number Nor A Consonant
		//If A Number Id Prime, It Is Only Divisible By 1 And Itself
		// Hence, If A Number Is Prime, The Value Of 'count' Will Be Exactly 2.
		//If The Value of 'count' Is Greater Than 2, The Number Is Divisible By Numbers Other Than 1 An Itself ANd Hence, It Is Not Prime
		// The value Of 'count' Will Be q Only if iArray[i] Is 1

		if (shubh_count == 2)
			printf("%d\n", shubh_iArray[shubh_i]);

		shubh_count = 0;
	}
	
	return 0;
}
