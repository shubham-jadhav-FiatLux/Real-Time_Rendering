#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_num;
	int* shubh_ptr = NULL;		// '*shubh_ptr' is a variable of type int
	int** shubh_pptr = NULL;

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

	shubh_pptr = &shubh_ptr;

	printf("\n\n");
	printf("******* After pptr = &ptr ********\n\n");
	printf("Value of num                                   = %d\n\n", shubh_num);
	printf("Address of 'num' (ptr)                         = % p\n\n", shubh_ptr);
	printf("Address of 'ptr' (pptr)                        = % p\n\n", shubh_pptr);
	printf("Value at address of 'ptr' (*pptr)              = %p\n\n", *shubh_pptr);
	printf("Value at address of 'num' (*ptr) (**pptr)      = %d\n\n", **shubh_pptr);

	return 0;
}

