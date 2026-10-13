#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	int MyStrlen(char[]);

	// Variable Declarations
	/*
	* *** A 'STRING' is an array of characters ... so char[] is a char array and hence, char[] is a 'string' ***
	* An array of char arrays is an array of strings !!!
	* Hence, char[] is one char array and hence, is one string
	* hence, char[][] is an array of char arrays and hence, is an array of strings
	*/

	// Here, the string array can allow a maximum number of 10 strings (10 rows) and each of these 10 strings can have only upto 15-characters maximum (15 columns)
	char shubh_strArray[10][15] = { "Hello!", "Welcome", "To", "Real", "Time", "Rendering", "Batch", "(2020-21)", "Of", "ASTROMEDICOMP." };		// In-line initialization
	int shubh_iStrLength[10];

	int shubh_shubh_strArray_size;
	int shubh_shubh_strArray_num_rows;
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	shubh_shubh_strArray_size = sizeof(shubh_strArray);
	shubh_shubh_strArray_num_rows = shubh_shubh_strArray_size / sizeof(shubh_strArray[0]);
	
	
	// Storing in length of all the strings ...
	for (shubh_i = 0; shubh_i < shubh_shubh_strArray_num_rows; shubh_i++) {

		shubh_iStrLength[shubh_i] = MyStrlen(shubh_strArray[shubh_i]);
	}
	
	printf("\n\n");
	printf("The Entire String Array : \n\n");
	for (shubh_i = 0; shubh_i < shubh_shubh_strArray_num_rows; shubh_i++) 

		printf("%s ",shubh_strArray[shubh_i]);

	printf("\n\n");
	printf("Strings In The 2D Array : \n\n");

	// Since, char[][] is an array of strings, referencing only by the row number (first[]) will give the row or the string
	// The column number (second []) is the particular charater in that string/row

	for (shubh_i = 0; shubh_i < shubh_shubh_strArray_num_rows; shubh_i++) {

		printf("String Number %d => %s\n\n", (shubh_i + 1), shubh_strArray[shubh_i]);
		for (shubh_j = 0; shubh_j < shubh_iStrLength[shubh_i]; shubh_j++) {

			printf("Character %d = %c\n", (shubh_j + 1), shubh_strArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	printf("\n\n");

	return 0;
}

int MyStrlen(char str[]) {

	// Variable Declarations
	int shubh_j;
	int shubh_string_length = 0;

	// Code
	// *** Determining exact length of the string, by detecting the first occurence of null-terminated charater '\0' ***
	for (shubh_j = 0; shubh_j < MAX_STRING_LENGTH; shubh_j++) {

		if (str[shubh_j] == '\0')
			break;
		else
			shubh_string_length++;
	}
	return shubh_string_length;
}
