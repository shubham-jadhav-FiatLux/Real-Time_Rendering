#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	int MyStrlen(char*);

	// Variable Declarations
	char *shubh_chArray = NULL;
	int shubh_iStringLength = 0;

	// Code
	printf("\n\n");
	shubh_chArray = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (shubh_chArray == NULL) {
	
		printf("Memory allocation to character array failed !!. Exiting ...\n\n");
		exit(0);
	}

	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	// ***** String Length *******
	printf("\n\n");
	shubh_iStringLength = MyStrlen(shubh_chArray);
	printf("Length of string is = %d characters !!\n\n", shubh_iStringLength);

	if (shubh_chArray) {
	
		free(shubh_chArray);
		shubh_chArray = NULL;
	}

	return 0;
}

int MyStrlen(char *str) {

	// Variable Declarations
	int shubh_j;
	int shubh_StringLength = 0;

	// Code
	// ***** Determining exact length of the string, By detecting thre first occurence of NULL-terminated character '\0' *****
	for (shubh_j = 0; shubh_j < MAX_STRING_LENGTH; shubh_j++) {

		if (*(str + shubh_j) == '\0')
			break;
		else
			shubh_StringLength++;
	}
	return(shubh_StringLength);
}
