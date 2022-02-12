#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Declarations
	void MathematicalOperations(int, int, int*, int*, int*, int*, int*);

	// Variable Declarations
	int shubh_a;
	int shubh_b;
	int *shubh_answer_sum = NULL;
	int *shubh_answer_difference = NULL;
	int *shubh_answer_product = NULL;
	int *shubh_answer_quotient = NULL;
	int *shubh_answer_remainder = NULL;

	// Code
	printf("\n\n");
	printf("Enter Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Value For 'B' : ");
	scanf("%d", &shubh_b);

	shubh_answer_sum = (int*)malloc(1 * sizeof(int));
	if (shubh_answer_sum == NULL){

		printf("Could Not Allocate Memory For 'shubh_answer_sum'! Exitting...\n\n");
		exit(0);
	}

	shubh_answer_difference = (int*)malloc(1 * sizeof(int));
	if (shubh_answer_difference == NULL) {

		printf("Could Not Allocate Memory For 'shubh_answer_difference'! Exitting...\n\n");
		exit(0);
	}

	shubh_answer_product = (int*)malloc(1 * sizeof(int));
	if (shubh_answer_product == NULL) {

		printf("Could Not Allocate Memory For 'shubh_answer_product'! Exitting...\n\n");
		exit(0);
	}

	shubh_answer_quotient = (int*)malloc(1 * sizeof(int));
	if (shubh_answer_quotient == NULL) {

		printf("Could Not Allocate Memory For 'shubh_answer_quotient'! Exitting...\n\n");
		exit(0);
	}

	shubh_answer_remainder = (int*)malloc(1 * sizeof(int));
	if (shubh_answer_remainder == NULL) {

		printf("Could Not Allocate Memory For 'shubh_answer_remainder'! Exitting...\n\n");
		exit(0);
	}

	MathematicalOperations(shubh_a, shubh_b, shubh_answer_sum, shubh_answer_difference, shubh_answer_product, shubh_answer_quotient, shubh_answer_remainder);

	printf("\n\n");
	printf("******* Results ****** : \n\n");
	printf("Sum			= %d\n\n", *shubh_answer_sum);
	printf("Difference  = %d\n\n", *shubh_answer_difference);
	printf("Product		= %d\n\n", *shubh_answer_product);
	printf("Quotient    = %d\n\n", *shubh_answer_quotient);
	printf("Remainder   = %d\n\n", *shubh_answer_remainder);

	if (shubh_answer_remainder) {
	
		free(shubh_answer_remainder);
		shubh_answer_remainder = NULL;
		printf("Memory allocated for shubh_answer_remainder  successfully freed !!\n\n");
	}

	if (shubh_answer_quotient) {

		free(shubh_answer_quotient);
		shubh_answer_quotient = NULL;
		printf("Memory allocated for shubh_answer_quotient  successfully freed !!\n\n");
	}

	if (shubh_answer_product) {

		free(shubh_answer_product);
		shubh_answer_product = NULL;
		printf("Memory allocated for shubh_answer_product  successfully freed !!\n\n");
	}

	if (shubh_answer_difference) {

		free(shubh_answer_difference);
		shubh_answer_difference = NULL;
		printf("Memory allocated for shubh_answer_difference  successfully freed !!\n\n");
	}

	if (shubh_answer_sum) {

		free(shubh_answer_sum);
		shubh_answer_sum = NULL;
		printf("Memory allocated for shubh_answer_sum  successfully freed !!\n\n");
	}

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
