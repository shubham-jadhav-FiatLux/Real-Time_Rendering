#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	int MyStrlen(char []);

	// Variable Declarations
	/*
	* *** A 'STRING' is an array of characters ... so char[] is a char array and hence, char[] is a 'string' ***
	* An array of char arrays is an array of strings !!!
	* Hence, char[] is one char array and hence, is one string
	* hence, char[][] is an array of char arrays and hence, is an array of strings
	*/

	// Here, the string array can allow a maximum number of 10 strings (10 rows) and each of these 10 strings can have only upto 15-characters maximum (15 columns)
	char shubh_strArray[10][15] = {"Hello!", "Welcome", "To", "Real", "Time", "Rendering", "Batch", "(2020-21)", "Of", "ASTROMEDICOMP."};		// In-line initialization
	int shubh_char_size;
	int shubh_shubh_strArray_size;
	int shubh_shubh_strArray_num_elements, shubh_shubh_strArray_num_rows, shubh_shubh_strArray_num_columns;
	int shubh_strActual_num_chars = 0;
	int shubh_i;

	// Code
	printf("\n\n");

	shubh_char_size = sizeof(char);

	shubh_shubh_strArray_size = sizeof(shubh_strArray);
	printf("Size Of Two Dimentional (2D) Character Array (String Array) Is = %d\n\n", shubh_shubh_strArray_size);

	shubh_shubh_strArray_num_rows = shubh_shubh_strArray_size / sizeof(shubh_strArray[0]);
	printf("Number Of Rows (Strings) In Two Dimentional (2D) Character Array (String Array) Is = %d\n\n", shubh_shubh_strArray_num_rows);

	shubh_shubh_strArray_num_columns = sizeof(shubh_strArray[0]) / shubh_char_size;
	printf("Number Of Columns In Two Dimentional (2D) Characeter Array (String Array) Is = %d\n\n", shubh_shubh_strArray_num_columns);

	shubh_shubh_strArray_num_elements = shubh_shubh_strArray_num_rows * shubh_shubh_strArray_num_columns;
	printf("Maximum Number Of Elements (Characters) In Two Dimentional (2D) Charater Array (String Array) Is = %d\n\n", shubh_shubh_strArray_num_elements);

	
	for (shubh_i = 0; shubh_i < shubh_shubh_strArray_num_rows; shubh_i++) {
	
		shubh_strActual_num_chars += MyStrlen(shubh_strArray[shubh_i]);
	}
	printf("Actual Number Of Elements (Characters) In Two Dimentional (2D) Characetr Array (String Array) Is = %d\n\n", shubh_strActual_num_chars);

	printf("\n\n");
	printf("Elements In The 2D Array : \n\n");

	// Since, char[][] is an array of strings, referencing only by the row number (first[]) will give the row or the string
	// The column number (second []) is the particular charater in that string/row

	printf("%s ", shubh_strArray[0]);
	printf("%s ", shubh_strArray[1]);
	printf("%s ", shubh_strArray[2]);
	printf("%s ", shubh_strArray[3]);
	printf("%s ", shubh_strArray[4]);
	printf("%s ", shubh_strArray[5]);
	printf("%s ", shubh_strArray[6]);
	printf("%s ", shubh_strArray[7]);
	printf("%s ", shubh_strArray[8]);
	printf("%s ", shubh_strArray[9]);

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
