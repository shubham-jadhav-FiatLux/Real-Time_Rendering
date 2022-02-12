#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH];
	int shubh_iStringLength;
	int shubh_i;
	int word_count = 0, space_count = 0;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	shubh_iStringLength = MyStrlen(shubh_chArray);

	for (shubh_i = 0; shubh_i < shubh_iStringLength; shubh_i++) {

		switch (shubh_chArray[shubh_i]) {

		case 32:
			space_count++;
			break;

		default:
			break;
		}
	}
	word_count = space_count + 1;

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	printf("\n\n");
	printf("Number of spaces in the input string = %d\n\n", space_count);
	printf("Number of words in the input string = %d\n\n", word_count);

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
