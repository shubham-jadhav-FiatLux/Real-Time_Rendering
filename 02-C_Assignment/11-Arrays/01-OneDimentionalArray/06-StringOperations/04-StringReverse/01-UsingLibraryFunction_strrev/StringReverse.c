#include <stdio.h>
#include <string.h>		// For strrev()

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_chArrayOriginal[MAX_STRING_LENGTH];

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArrayOriginal, MAX_STRING_LENGTH);

	// String Output
	printf("\n\n");
	printf("The original string entered by you is : \n\n");
	printf("%s\n", shubh_chArrayOriginal);

	printf("\n\n");
	printf("The Reversed String is : \n\n");
	printf("%s\n", strrev(shubh_chArrayOriginal));

	return 0;
}
