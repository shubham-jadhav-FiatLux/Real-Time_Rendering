#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	double shubh_num;
	double* shubh_ptr = NULL;

	// Code
	shubh_num = 2.343434f;

	printf("\n\n");
	printf("******* Before ptr = &num ********\n\n");
	printf("Value of num                  = %lf\n\n", shubh_num);
	printf("Address of num                = %p\n\n", &shubh_num);
	printf("Value at address of num       = %lf\n\n", *(&shubh_num));

	shubh_ptr = &shubh_num;

	printf("\n\n");
	printf("******* After ptr = &num ********\n\n");
	printf("Value of num                  = %lf\n\n", shubh_num);
	printf("Address of num                = %p\n\n", shubh_ptr);
	printf("Value at address of num       = %lf\n\n", *shubh_ptr);

	return 0;
}

