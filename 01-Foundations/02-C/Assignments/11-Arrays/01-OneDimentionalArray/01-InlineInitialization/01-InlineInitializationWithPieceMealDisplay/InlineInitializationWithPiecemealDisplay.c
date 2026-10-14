// Array Inline Initialization With Piecemeal Method

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[] = {9, 30, 6, 12, 98, 95, 20, 23, 2, 45};
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements;

	float shubh_fArray[] = { 1.2f, 2.3f, 3.4f, 4.5, 5.6f, 6.7f, 7.8f, 8.9f};
	int shubh_float_size;
	int shubh_fArray_size;
	int shubh_fArray_num_elements;

	char shubh_cArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P'};
	int shubh_char_size;
	int shubh_cArray_size;
	int shubh_cArray_num_elements;

	// Code

	//***** shubh_iArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_iArray' : \n\n");
	printf("shubh_iArray[0] (1st Element) = %d\n", shubh_iArray[0]);
	printf("shubh_iArray[1] (2st Element) = %d\n", shubh_iArray[1]);
	printf("shubh_iArray[2] (3st Element) = %d\n", shubh_iArray[2]);
	printf("shubh_iArray[3] (4st Element) = %d\n", shubh_iArray[3]);
	printf("shubh_iArray[4] (5st Element) = %d\n", shubh_iArray[4]);
	printf("shubh_iArray[5] (6st Element) = %d\n", shubh_iArray[5]);
	printf("shubh_iArray[6] (7st Element) = %d\n", shubh_iArray[6]);
	printf("shubh_iArray[7] (8st Element) = %d\n", shubh_iArray[7]);
	printf("shubh_iArray[8] (9st Element) = %d\n", shubh_iArray[8]);
	printf("shubh_iArray[9] (10st Element) = %d\n", shubh_iArray[9]);

	shubh_int_size = sizeof(int);
	shubh_iArray_size = sizeof(shubh_iArray);
	shubh_iArray_num_elements = shubh_iArray_size / shubh_int_size;
	printf("Size Of Data Type 'int'                                         = %d bytes\n", shubh_int_size);
	printf("Number Of Elements In 'int' Array 'shubh_iArray[]'              = %d bytes\n", shubh_iArray_num_elements);
	printf("Size Of Array 'shubh_iArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_iArray_num_elements, shubh_int_size, shubh_iArray_size);
	
	

	//***** shubh_fArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_fArray' : \n\n");
	printf("shubh_fArray[0] (1st Element) = %f\n", shubh_fArray[0]);
	printf("shubh_fArray[1] (2st Element) = %f\n", shubh_fArray[1]);
	printf("shubh_fArray[2] (3st Element) = %f\n", shubh_fArray[2]);
	printf("shubh_fArray[3] (4st Element) = %f\n", shubh_fArray[3]);
	printf("shubh_fArray[4] (5st Element) = %f\n", shubh_fArray[4]);
	printf("shubh_fArray[5] (6st Element) = %f\n", shubh_fArray[5]);
	printf("shubh_fArray[6] (7st Element) = %f\n", shubh_fArray[6]);
	printf("shubh_fArray[7] (8st Element) = %f\n", shubh_fArray[7]);
	printf("shubh_fArray[8] (9st Element) = %\n", shubh_fArray[8]);
	printf("shubh_fArray[9] (10st Element) = %f\n", shubh_fArray[9]);

	shubh_float_size = sizeof(float);
	shubh_fArray_size = sizeof(shubh_fArray);
	shubh_fArray_num_elements = shubh_fArray_size / shubh_float_size;
	printf("Size Of Data Type 'float'                                       = %d bytes\n", shubh_float_size);
	printf("Number Of Elements In 'float' Array 'shubh_fArray[]'            = %d bytes\n", shubh_fArray_num_elements);
	printf("Size Of Array 'shubh_fArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_fArray_num_elements, shubh_float_size, shubh_fArray_size);


	//***** shubh_cArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_cArray' : \n\n");
	printf("shubh_cArray[0] (1st Element) = %c\n", shubh_cArray[0]);
	printf("shubh_cArray[1] (2st Element) = %c\n", shubh_cArray[1]);
	printf("shubh_cArray[2] (3st Element) = %c\n", shubh_cArray[2]);
	printf("shubh_cArray[3] (4st Element) = %c\n", shubh_cArray[3]);
	printf("shubh_cArray[4] (5st Element) = %c\n", shubh_cArray[4]);
	printf("shubh_cArray[5] (6st Element) = %c\n", shubh_cArray[5]);
	printf("shubh_cArray[6] (7st Element) = %c\n", shubh_cArray[6]);
	printf("shubh_cArray[7] (8st Element) = %c\n", shubh_cArray[7]);
	printf("shubh_cArray[8] (9st Element) = %c\n", shubh_cArray[8]);
	printf("shubh_cArray[9] (10st Element) = %c\n", shubh_cArray[9]);

	shubh_char_size = sizeof(char);
	shubh_cArray_size = sizeof(shubh_cArray);
	shubh_cArray_num_elements = shubh_cArray_size / shubh_char_size;
	printf("Size Of Data Type 'char'                                        = %d bytes\n", shubh_char_size);
	printf("Number Of Elements In 'char' Array 'shubh_cArray[]'             = %d bytes\n", shubh_cArray_num_elements);
	printf("Size Of Array 'shubh_cArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_cArray_num_elements, shubh_char_size, shubh_cArray_size);

	return 0;
}
