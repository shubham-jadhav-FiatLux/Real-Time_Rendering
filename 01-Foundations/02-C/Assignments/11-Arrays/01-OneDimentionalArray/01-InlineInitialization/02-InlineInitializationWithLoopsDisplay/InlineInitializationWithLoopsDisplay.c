// Array Inline Initialization With Piecemeal Method

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_iArray[] = { 9, 30, 6, 12, 98, 95, 20, 23, 2, 45 };
	int shubh_int_size;
	int shubh_iArray_size;
	int shubh_iArray_num_elements;

	float shubh_fArray[] = { 1.2f, 2.3f, 3.4f, 4.5, 5.6f, 6.7f, 7.8f, 8.9f };
	int shubh_float_size;
	int shubh_fArray_size;
	int shubh_fArray_num_elements;

	char shubh_cArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P' };
	int shubh_char_size;
	int shubh_cArray_size;
	int shubh_cArray_num_elements;

	// Code

	//***** shubh_iArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_iArray' : \n\n");
	
	shubh_int_size = sizeof(int);
	shubh_iArray_size = sizeof(shubh_iArray);
	shubh_iArray_num_elements = shubh_iArray_size / shubh_int_size;
	
	for (int shubh_i = 1; shubh_i < shubh_iArray_num_elements; shubh_i++) {

		printf("shubh_iArray[%d] (Element %2d)  =  %2d\n", shubh_i, (shubh_i + 1), shubh_iArray[shubh_i]);
	}
	printf("\n\n");
	printf("Size Of Data Type 'int'                                         = %d bytes\n", shubh_int_size);
	printf("Number Of Elements In 'int' Array 'shubh_iArray[]'              = %d bytes\n", shubh_iArray_num_elements);
	printf("Size Of Array 'shubh_iArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_iArray_num_elements, shubh_int_size, shubh_iArray_size);



	//***** shubh_fArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_fArray' : \n\n");
	
	shubh_float_size = sizeof(float);
	shubh_fArray_size = sizeof(shubh_fArray);
	shubh_fArray_num_elements = shubh_fArray_size / shubh_float_size;
	
	for (int shubh_i = 0; shubh_i < shubh_fArray_num_elements; shubh_i++) {

		printf("shubh_fArray[%d] (Element %2d)  =  %f\n", shubh_i, (shubh_i + 1), shubh_fArray[shubh_i]);
	}
	printf("\n\n");
	printf("Size Of Data Type 'float'                                       = %d bytes\n", shubh_float_size);
	printf("Number Of Elements In 'float' Array 'shubh_fArray[]'            = %d bytes\n", shubh_fArray_num_elements);
	printf("Size Of Array 'shubh_fArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_fArray_num_elements, shubh_float_size, shubh_fArray_size);



	//***** shubh_cArray *****
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements Of Array 'shubh_cArray' : \n\n");

	shubh_char_size = sizeof(char);
	shubh_cArray_size = sizeof(shubh_cArray);
	shubh_cArray_num_elements = shubh_cArray_size / shubh_char_size;

	for (int shubh_i = 0; shubh_i < shubh_cArray_num_elements; shubh_i++) {
	
		printf("shubh_cArray[%d] (Element %2d)  =  %c\n", shubh_i, (shubh_i + 1), shubh_cArray[shubh_i]);
	}
	printf("\n\n");
	printf("Size Of Data Type 'char'                                        = %d bytes\n", shubh_char_size);
	printf("Number Of Elements In 'char' Array 'shubh_cArray[]'             = %d bytes\n", shubh_cArray_num_elements);
	printf("Size Of Array 'shubh_cArray[]' (%d Elements * %d Bytes)         = %d bytes\n\n", shubh_cArray_num_elements, shubh_char_size, shubh_cArray_size);

	return 0;
}
