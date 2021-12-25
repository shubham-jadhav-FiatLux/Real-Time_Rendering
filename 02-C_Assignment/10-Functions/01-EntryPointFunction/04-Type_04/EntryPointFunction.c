// Type 4 Of Writing Entry Point Function

#include <stdio.h>

int main(int argc, char* argv[]) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");
	printf("Hello, World !!\n\n");
	printf("Number Of Command Line Arguments = %d\n\n", argc);

	printf("Command Line Arguments Passed To This Program Are : \n\n");

	for (shubh_i = 0; shubh_i < argc; shubh_i++) {
	
		printf("Command Line Argument Number %d = %s\n", (shubh_i + 1), argv[shubh_i]);
	}

	printf("\n\n");

	return 0;
}
