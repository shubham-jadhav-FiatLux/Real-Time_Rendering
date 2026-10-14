#include <stdio.h>
#include <ctype.h>		// For toupper()

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	int MyStrlen(char[]);

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH], shubh_chArrayCapitalized[MAX_STRING_LENGTH];
	int shubh_iStringLength;
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	shubh_iStringLength = MyStrlen(shubh_chArray);
	shubh_j = 0;
	for (shubh_i = 0; shubh_i < shubh_iStringLength; shubh_i++) {

		if (shubh_i == 0)
			shubh_chArrayCapitalized[shubh_j] = toupper(shubh_chArray[shubh_i]);
		
		else if (shubh_chArray[shubh_i] == ' ') {
		
			shubh_chArrayCapitalized[shubh_j] = shubh_chArray[shubh_i];
			shubh_chArrayCapitalized[shubh_j + 1] = toupper(shubh_chArray[shubh_i + 1]);
			shubh_j++;
			shubh_i++;
		}

		else 
			shubh_chArrayCapitalized[shubh_j] = shubh_chArray[shubh_i];
		
		shubh_j++;
	}
	shubh_chArrayCapitalized[shubh_j] = '\0';

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	printf("\n\n");
	printf("String after capitalizing first letter of every word : \n\n");
	printf("%s\n", shubh_chArrayCapitalized);

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
