#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	int MyStrlen(char[]);

	// Variable Declarations
	char shubh_chArray[MAX_STRING_LENGTH];
	int shubh_iStringLength;
	int count_A = 0, count_E = 0, count_I = 0, count_O = 0, count_U = 0;
	int shubh_i;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(shubh_chArray, MAX_STRING_LENGTH);

	printf("\n\n");
	printf("String entered by you is : \n\n");
	printf("%s\n", shubh_chArray);

	shubh_iStringLength = MyStrlen(shubh_chArray);

	for (shubh_i = 0; shubh_i < shubh_iStringLength; shubh_i++) {
	
		switch (shubh_chArray[shubh_i]) {
		
		case 'A':
		case 'a':
			count_A++;
			break;

		case 'E':
		case 'e':
			count_E++;
			break;

		case 'I':
		case 'i':
			count_I++;
			break;

		case 'O':
		case 'o':
			count_O++;
			break;

		case 'U':
		case 'u':
			count_U++;
			break;

		default:
			break;
		}
	}

	printf("\n\n");
	printf("In the string entered by you, The vowels and the number of their occurences are follows : \n\n");
	printf("'A' Has occured = %d times !!\n\n", count_A);
	printf("'E' Has occured = %d times !!\n\n", count_E);
	printf("'I' Has occured = %d times !!\n\n", count_I);
	printf("'O' Has occured = %d times !!\n\n", count_O);
	printf("'U' Has occured = %d times !!\n\n", count_U);
	
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
