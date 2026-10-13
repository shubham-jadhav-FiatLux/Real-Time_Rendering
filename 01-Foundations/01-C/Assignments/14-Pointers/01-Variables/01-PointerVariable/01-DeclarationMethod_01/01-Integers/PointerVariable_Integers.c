#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_num;
	int* shubh_ptr = NULL;

	// Code
	shubh_num = 10;

	printf("\n\n");
	printf("******* Before ptr = &num ********\n\n");
	printf("Value of num                  = %d\n\n", shubh_num);
	printf("Address of num                = % p\n\n", &shubh_num);
	printf("Value at address of num       = %d\n\n", *(&shubh_num));

	shubh_ptr = &shubh_num;

	printf("\n\n");
	printf("******* After ptr = &num ********\n\n");
	printf("Value of num                  = %d\n\n", shubh_num);
	printf("Address of num                = % p\n\n", shubh_ptr);
	printf("Value at address of num       = %d\n\n", *shubh_ptr);

	return 0;
}

