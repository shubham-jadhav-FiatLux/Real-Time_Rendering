#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Declarations
	void MathematicalOperations(int , int, int*, int*, int*, int*, int*);

	// Variable Declarations
	int shubh_a;
	int shubh_b;
	int shubh_answer_sum;
	int shubh_answer_difference;
	int shubh_answer_product;
	int shubh_answer_quotient;
	int shubh_answer_remainder;

	// Code
	printf("\n\n");
	printf("Enter Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Value For 'B' : ");
	scanf("%d", &shubh_b);

	MathematicalOperations(shubh_a, shubh_b, &shubh_answer_sum, &shubh_answer_difference, &shubh_answer_product, &shubh_answer_quotient, &shubh_answer_remainder);
	
	printf("\n\n");
	printf("******* Results ****** : \n\n");
	printf("Sum			= %d\n\n", shubh_answer_sum);
	printf("Difference  = %d\n\n", shubh_answer_difference);
	printf("Product		= %d\n\n", shubh_answer_product);
	printf("Quotient    = %d\n\n", shubh_answer_quotient);
	printf("Remainder   = %d\n\n", shubh_answer_remainder);

	return 0;
}

void MathematicalOperations(int x, int y, int* sum, int* difference, int* product, int* quotient, int* remainder) {

	// Code
	*sum = x + y;
	*difference = x - y;
	*product = x * y;
	*quotient = x / y;
	*remainder = x % y;
}
