#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void MyStrcpy(char[], char[]);

	// Variable Declarations
	/*
	* *** A 'STRING' is an array of characters ... so char[] is a char array and hence, char[] is a 'string' ***
	* An array of char arrays is an array of strings !!!
	* Hence, char[] is one char array and hence, is one string
	* hence, char[][] is an array of char arrays and hence, is an array of strings
	*/

	// Here, the string array can allow a maximum number of 5 strings (5 rows) and each of these 5 strings can have only upto 10-characters maximum (10 columns)
	char shubh_strArray[5][10];		//
	int shubh_char_size;
	int shubh_shubh_strArray_size;
	int shubh_shubh_strArray_num_elements, shubh_shubh_strArray_num_rows, shubh_shubh_strArray_num_columns;
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


	// *** PIECE-MEAL ASSIGNMENT ***
	// ***** ROW 1 / STRING 1 *****
	shubh_strArray[0][0] = 'M';
	shubh_strArray[0][1] = 'y';
	shubh_strArray[0][2] = '\0';

	// ***** ROW 2 / STRING 2 *****
	shubh_strArray[1][0] = 'N';
	shubh_strArray[1][1] = 'a';
	shubh_strArray[1][2] = 'm';
	shubh_strArray[1][3] = 'e';
	shubh_strArray[1][4] = '\0';

	// ***** ROW 3 / STRING 3 *****
	shubh_strArray[2][0] = 'I';
	shubh_strArray[2][1] = 's';
	shubh_strArray[2][2] = '\0';

	// ***** ROW 4 / STRING 4 *****
	shubh_strArray[3][0] = 'S';
	shubh_strArray[3][1] = 'h';
	shubh_strArray[3][2] = 'u';
	shubh_strArray[3][3] = 'b';
	shubh_strArray[3][4] = 'h';
	shubh_strArray[3][5] = 'a';
	shubh_strArray[3][6] = 'm';
	shubh_strArray[3][7] = '\0';

	// ***** ROW 5 / STRING 5 *****
	shubh_strArray[4][0] = 'J';
	shubh_strArray[4][1] = 'a';
	shubh_strArray[4][2] = 'd';
	shubh_strArray[4][3] = 'h';
	shubh_strArray[4][4] = 'a';
	shubh_strArray[4][5] = 'v';
	shubh_strArray[4][6] = '\0';

	printf("\n\n");
	printf("Strings In The 2D Character Array : \n\n");

	for (shubh_i = 0; shubh_i < shubh_shubh_strArray_num_rows; shubh_i++) {
		printf("%s ", shubh_strArray[shubh_i]);
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
