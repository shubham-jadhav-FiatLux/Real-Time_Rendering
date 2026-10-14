// Program For User Input Dependent Iteration In 'while' Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_start, shubh_num, shubh_i;

	// Code
	printf("\n\n");

	printf("Enter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &shubh_start);

	printf("How Many Digits Do You Want To Print From %d Onwards ? : ", shubh_start);
	scanf("%d", &shubh_num);

	printf("\n\n");

	shubh_i = shubh_start;

	while (shubh_i <= (shubh_start + shubh_num)) {
	
		printf("\t%d\n", shubh_i);
		shubh_i++;
	}

	printf("\n\n");

	return 0;
}