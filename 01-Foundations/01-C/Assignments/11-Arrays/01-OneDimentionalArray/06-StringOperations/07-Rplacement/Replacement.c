#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	// Variable Declarations
	char shubh_chArrayOriginal[MAX_STRING_LENGTH], shubh_chArrayVowelIsReplaced[MAX_STRING_LENGTH];
	int shubh_iStringLength;
	int shubh_i;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArrayOriginal, MAX_STRING_LENGTH);

	MyStrcpy(shubh_chArrayVowelIsReplaced, shubh_chArrayOriginal);
	shubh_iStringLength = MyStrlen(shubh_chArrayVowelIsReplaced);

	for (shubh_i = 0; shubh_i < shubh_iStringLength; shubh_i++) {

		switch (shubh_chArrayVowelIsReplaced[shubh_i]) {

		case 'A':
		case 'a':
		case 'E':
		case 'e':
		case 'I':
		case 'i':
		case 'O':
		case 'o':
		case 'U':
		case 'u':
			shubh_chArrayVowelIsReplaced[shubh_i] = '*';
			break;

		default:
			break;
		}
	}

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArrayOriginal);

	printf("\n\n");
	printf("String after replacent of vowel by * is : \n\n");
	printf("%s\n", shubh_chArrayVowelIsReplaced);
	
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

void MyStrcpy(char str_destination[], char str_source[]) {

	// Function Prototype
	int MyStrlen(char[]);

	// Variable Declarations
	int shubh_iStringLength = 0;
	int shubh_j;

	// Code
	shubh_iStringLength = MyStrlen(str_source);
	for (shubh_j = 0; shubh_j < shubh_iStringLength; shubh_j++)
		str_destination[shubh_j] = str_source[shubh_j];

	str_destination[shubh_j] = '\0';
}
