#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH];		// A character array is a string

	// Code
	printf("\n\n");
	printf("Enter A String : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("String Entered By You Is : \n\n");
	printf("%s\n", shubh_chArray);

	return 0;
}
