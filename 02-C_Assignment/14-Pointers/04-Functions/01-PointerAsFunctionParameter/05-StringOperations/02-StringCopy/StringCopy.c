#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void MyStrcpy(char*, char*);
	int MyStrlen(char *);

	// Variable Declarations
	char *shubh_chArrayOriginal = NULL, *shubh_chArrayCopy = NULL;
	int origital_string_length;

	// Code
	printf("\n\n");
	shubh_chArrayOriginal = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (shubh_chArrayOriginal == NULL) {

		printf("Memory allocation for original string failed !!. Exiting ...\n\n");
		exit(0);
	}
	printf("Enter a string : \n\n");
	gets_s(shubh_chArrayOriginal, MAX_STRING_LENGTH);

	origital_string_length = MyStrlen(shubh_chArrayOriginal);
	shubh_chArrayCopy = (char*)malloc(origital_string_length * sizeof(char));
	if (shubh_chArrayCopy == NULL) {

		printf("Memory allocation for copied string failed !!. Exiting ...\n\n");
		exit(0);
	}


	// *** String Copy ****
	MyStrcpy(shubh_chArrayCopy, shubh_chArrayOriginal);

	// String Output
	printf("\n\n");
	printf("The original string entered by you is : \n\n");
	printf("%s\n", shubh_chArrayOriginal);

	printf("\n\n");
	printf("The Copied String is : \n\n");
	printf("%s\n", shubh_chArrayCopy);

	if (shubh_chArrayCopy) {
	
		free(shubh_chArrayCopy);
		shubh_chArrayCopy = NULL;
		printf("Memory allocated for copied string has been successfully freed !!\n\n");
	}

	if (shubh_chArrayOriginal) {

		free(shubh_chArrayOriginal);
		shubh_chArrayOriginal = NULL;
		printf("Memory allocated for Original string has been successfully freed !!\n\n");
	}

	return 0;
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
