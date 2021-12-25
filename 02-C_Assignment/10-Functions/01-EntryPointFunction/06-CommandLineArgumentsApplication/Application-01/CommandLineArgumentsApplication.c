// Command Line Arguments Application

#include <stdio.h>		// It Contains Declaration Of 'printf()'
#include <ctype.h>		// It Contains Declaration Of 'atoi()'
#include <stdlib.h>		// It Contains Declaration Of 'exit()'

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_i;
	int shubh_num;
	int shubh_sum = 0;

	// Code
	if (argc == 1) {
	
		printf("\n\n");
		printf("No Number Given For Addition !! Exitting Now ...\n\n");
		printf("Usage : CommandLineArgumentsApplication <first number> <second number> ...\n\n");
		exit(0);
	}

	//*** This Program Adds All Command Line Arguments Given In Integer Form Only And Outputs The Sum *** 
	//*** Due To Use Of atoi(), All Command Line Arguments Of Types Other Than 'int' Are Ignored ***

	printf("\n\n");
	printf("Sum Of All Integer Command Line Arguments Is : \n\n");

	// Loop Starts From i=1 Because, i=0 Will Result In 'argv[i]' = 'argv[0]' Which Is The Name Of The Program Itself i.e. : 'ComandLineArgumentsApplication.exe'

	for (shubh_i = 1; shubh_i < argc; shubh_i++) {
	
		shubh_num = atoi(argv[shubh_i]);
		shubh_sum += shubh_num;
	}

	printf("shubh_sum = %d\n\n", shubh_sum);

	return 0;
}
