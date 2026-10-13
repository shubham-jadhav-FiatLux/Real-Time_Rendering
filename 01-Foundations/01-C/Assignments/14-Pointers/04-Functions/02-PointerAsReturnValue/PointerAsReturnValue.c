#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	char* ReplaceVowelsWithHashSymbol(char*);

	// Variable declaration
	char string[MAX_STRING_LENGTH];
	char* replaced_string = NULL;
	
	// Code
	printf("\n\n");
	printf("Enter String : ");
	gets_s(string, MAX_STRING_LENGTH);
	
	replaced_string = ReplaceVowelsWithHashSymbol(string);
	
	if (replaced_string == NULL) {

		printf("ReplaceVowelsWithHashSymbol() Function Has Failed !! Exitiing...\n\n");
		exit(0);
	}
	printf("\n\n");
	printf("Replaced String Is : \n\n");
	printf("%s\n\n", replaced_string);

	if (replaced_string) {

		free(replaced_string);
		replaced_string = NULL;
	}

	return 0;
}

char* ReplaceVowelsWithHashSymbol(char* s) {

	// Function prototype
	void MyStrcpy(char*, char*);
	int MyStrlen(char*);
	
	// Varibale Declarations
	char* new_string = NULL;
	int i;

	// Code
	new_string = (char*)malloc(MyStrlen(s) * sizeof(char));
	if (new_string == NULL) {

		printf("Could not allocate memory for new string !!\n\n");
		return NULL;
	}

	MyStrcpy(new_string, s);
	for (i = 0; i < MyStrlen(new_string); i++) {

		switch (new_string[i]) {

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
			new_string[i] = '#';
			break;
		
		default:
			break;
		}
	}
	return new_string;
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
