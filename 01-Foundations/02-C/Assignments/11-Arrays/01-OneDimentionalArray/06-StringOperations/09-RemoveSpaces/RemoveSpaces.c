#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	int MyStrlen(char[]);

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH], shubh_chArraySpacesRemoved[MAX_STRING_LENGTH];
	int shubh_iStringLength;
	int shubh_i, shubh_j;
	
	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	shubh_iStringLength = MyStrlen(shubh_chArray);
	shubh_j = 0;
	for (shubh_i = 0; shubh_i < shubh_iStringLength; shubh_i++) {

		if (shubh_chArray[shubh_i] == ' ')
			continue;
		else {
		
			shubh_chArraySpacesRemoved[shubh_j] = shubh_chArray[shubh_i];
			shubh_j++;
		}
	}
	shubh_chArraySpacesRemoved[shubh_j] = '\0';

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	printf("\n\n");
	printf("String after removal of spaces is : \n\n");
	printf("%s\n", shubh_chArraySpacesRemoved);

	return 0;
}

int MyStrlen(char str[]) {

	// Variable Declarations
	int shubh_j;
	int shubh_StringLength = 0;

	// Code
	// ***** Determining exact length of the string, By detecting thre first occurence of NULL-terminated character '\0' *****
	for (shubh_j = 0; shubh_j < MAX_STRING_LENGTH; shubh_j++) {

		if (str[shubh_j] == '\0')
			break;
		else
			shubh_StringLength++;
	}
	return(shubh_StringLength);
}
