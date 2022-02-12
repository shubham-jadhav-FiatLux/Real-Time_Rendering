#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	void MyStrcat(char[], char[]);

	// Variable Declarations
	char shubh_chArrayOne[MAX_STRING_LENGTH], shubh_chArrayTwo[MAX_STRING_LENGTH];

	// Code
	printf("\n\n");
	printf("Enter First string : \n\n");
	gets_s(shubh_chArrayOne, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("Enter Second string : \n\n");
	gets_s(shubh_chArrayTwo, MAX_STRING_LENGTH);

	// String Concat
	printf("\n\n");
	printf("******* Before Concatination ********");
	printf("\n\n");
	printf("The original First string entered by you is : \n\n");
	printf("%s\n", shubh_chArrayOne);

	printf("\n\n");
	printf("The original Second string entered by you is : \n\n");
	printf("%s\n", shubh_chArrayTwo);

	MyStrcat(shubh_chArrayOne, shubh_chArrayTwo);

	printf("\n\n");
	printf("******* After Concatination ********");
	printf("\n\n");
	printf(" 'shubh_chArrayOne[]' is : \n\n");
	printf("%s\n", shubh_chArrayOne);

	printf("\n\n");
	printf(" 'shubh_chArrayTwo[]' is : \n\n");
	printf("%s\n", shubh_chArrayTwo);

	return 0;
}

void MyStrcat(char str_destination[], char str_source[]) {

	// Function Prototype
	int MyStrlen(char[]);

	// Variable Declarations
	int shubh_iStringLengthSource = 0, shubh_iStringLengthDestination = 0;
	int shubh_i, shubh_j;

	// Code
	shubh_iStringLengthSource = MyStrlen(str_source);
	shubh_iStringLengthDestination = MyStrlen(str_destination);

	for (shubh_i = shubh_iStringLengthDestination, shubh_j = 0; shubh_j < shubh_iStringLengthSource ; shubh_i++, shubh_j++)
		str_destination[shubh_i] = str_source[shubh_j];

	str_destination[shubh_i] = '\0';
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