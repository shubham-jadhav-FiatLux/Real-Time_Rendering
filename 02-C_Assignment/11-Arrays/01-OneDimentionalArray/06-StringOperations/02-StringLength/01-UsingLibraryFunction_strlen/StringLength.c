#include <stdio.h>
//#include <string.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH];
	int shubh_iStringLength = 0;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	// ***** String Length *******
	printf("\n\n");
	shubh_iStringLength = strlen(shubh_chArray);
	printf("Length of string is = %d characters !!\n\n", shubh_iStringLength);

	return 0;
}
