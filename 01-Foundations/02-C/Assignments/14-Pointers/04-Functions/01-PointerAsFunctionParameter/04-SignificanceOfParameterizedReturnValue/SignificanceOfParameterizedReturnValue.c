#include <stdio.h>

enum {

	NEGATIVE = -1,
	ZERO,
	POSITIVE
};

int main(int argc, char* argv[], char* envp[]) {

	// Function Declarations
	int Difference(int, int, int*);

	// Variable Declarations
	int shubh_a;
	int shubh_b;
	int shubh_answer;
	int shubh_ret;
	
	// Code
	printf("\n\n");
	printf("Enter Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Value For 'B' : ");
	scanf("%d", &shubh_b);

	shubh_ret = Difference(shubh_a, shubh_b, &shubh_answer);

	printf("\n\n");
	printf("Difference of %d and %d = %d\n\n", shubh_a, shubh_b, shubh_answer);

	if (shubh_ret == POSITIVE)
		printf("Difference of %d and %d is Positive !!\n\n", shubh_a, shubh_b);

	else if (shubh_ret == NEGATIVE)
		printf("Difference of %d and %d is Negative !!\n\n", shubh_a, shubh_b);

	else
		printf("Difference of %d and %d is Zero !!\n\n", shubh_a, shubh_b);

	return 0;
}

int Difference(int x, int y, int* difference) {

	// Code
	*difference = x - y;
	
	if (*difference > 0)
		return POSITIVE;

	else if (*difference < 0)
		return NEGATIVE;

	else
		return ZERO;
}
