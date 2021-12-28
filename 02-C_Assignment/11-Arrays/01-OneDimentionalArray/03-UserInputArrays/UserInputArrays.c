// program To Take Array Elements From User

#include <stdio.h>

// Macro Constant Used As Array Size In Subscript And As Array Length.
// Hence, This Program's Array's Sizes Can Be  Simply Changed By Changing Following 3 Global Macro Constant Values, Before Compiling, Linking And Executing The Program !!


#define SHUBH_INT_ARRAY_NUM_ELEMENTS 5
#define SHUBH_FLOAT_ARRAY_NUM_ELEMENTS 3
#define SHUBH_CHAR_ARRAY_NUM_ELEMENTS 15

int main(void) {

	// Variable Declarations
	int shubh_iArray[SHUBH_INT_ARRAY_NUM_ELEMENTS];
	float shubh_fArray[SHUBH_FLOAT_ARRAY_NUM_ELEMENTS];
	char shubh_cArray[SHUBH_CHAR_ARRAY_NUM_ELEMENTS];
	int shubh_i, shubh_num;

	// Code
	
	// ********* ARRAY ELEMENTS INPUT ********
	printf("\n\n");
	printf("Enter Elements For Integer Array : \n");
	for (shubh_i = 0; shubh_i < SHUBH_INT_ARRAY_NUM_ELEMENTS; shubh_i++) {
	
		scanf("%d", &shubh_iArray[shubh_i]);
	}

	printf("\n\n");
	printf("Enter Elements For Floating Point Array : \n");
	for (shubh_i = 0; shubh_i < SHUBH_FLOAT_ARRAY_NUM_ELEMENTS; shubh_i++) {

		scanf("%f", &shubh_fArray[shubh_i]);
	}

	printf("\n\n");
	printf("Enter Elements For Character Array : \n");
	for (shubh_i = 0; shubh_i < SHUBH_CHAR_ARRAY_NUM_ELEMENTS; shubh_i++) {

		shubh_cArray[shubh_i] = getch();
		printf("%c\n", shubh_cArray[shubh_i]);
	}

	// ********* ARRAY ELEMENTS OUTPUT ********
	printf("\n\n");
	printf("Integer Array Entered By You : \n\n");
	for (shubh_i = 0; shubh_i < SHUBH_INT_ARRAY_NUM_ELEMENTS; shubh_i++) {

		printf("%d\n", shubh_iArray[shubh_i]);
	}

	printf("\n\n");
	printf("Floating Point Array Entered By You : \n\n");
	for (shubh_i = 0; shubh_i < SHUBH_FLOAT_ARRAY_NUM_ELEMENTS; shubh_i++) {

		printf("%f\n", shubh_fArray[shubh_i]);
	}
	
	printf("\n\n");
	printf("Character Array Entered By You : \n\n");
	for (shubh_i = 0; shubh_i < SHUBH_CHAR_ARRAY_NUM_ELEMENTS; shubh_i++) {

		printf("%c\n", shubh_cArray[shubh_i]);
	}

	return 0;
}
