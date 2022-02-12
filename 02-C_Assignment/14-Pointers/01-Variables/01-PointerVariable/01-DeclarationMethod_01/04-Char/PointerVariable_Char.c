#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_ch;
	char* shubh_ptr = NULL;

	// Code
	shubh_ch = 'A';

	printf("\n\n");
	printf("******* Before ptr = &ch ********\n\n");
	printf("Value of ch                 = %c\n\n", shubh_ch);
	printf("Address of ch               = %p\n\n", &shubh_ch);
	printf("Value at address of ch      = %c\n\n", *(&shubh_ch));

	shubh_ptr = &shubh_ch;

	printf("\n\n");
	printf("******* After ptr = &ch ********\n\n");
	printf("Value of ch                  = %c\n\n", shubh_ch);
	printf("Address of ch                = %p\n\n", shubh_ptr);
	printf("Value at address of ch       = %c\n\n", *shubh_ptr);

	return 0;
}

