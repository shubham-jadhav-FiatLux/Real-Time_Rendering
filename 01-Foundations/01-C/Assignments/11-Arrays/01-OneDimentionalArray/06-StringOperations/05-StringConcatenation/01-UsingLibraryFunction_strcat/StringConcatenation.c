#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

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

	strcat(shubh_chArrayOne, shubh_chArrayTwo);

	printf("\n\n");
	printf("******* After Concatination ********");
	printf("\n\n");
	printf(" 'shubh_chArrayOne[]' is : \n\n");
	printf("%s\n", shubh_chArrayOne);

	printf("\n\n");
	printf(" 'shubh_chArrayOne[]' is : \n\n");
	printf("%s\n", shubh_chArrayTwo);

	return 0;
}
