// Program For Different Methods Of Characterd Array Declaration

#include <stdio.h>

int main(void) {

	// Variable Declarations
	char shubh_chArray_01[] = {'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0'};	//Must Give '\0' Explicitly For Proper Initialization
	char shubh_chArray_02[] = {'W' ,'E', 'L', 'C', 'O', 'M', 'E', 'S', '\0'};							//Must Give '\0' Explicitly For Proper Initialization
	char shubh_chArray_03[] = {'Y', 'O', 'U', '\0'};													//Must Give '\0' Explicitly For Proper Initialization
	char shubh_chArray_04[] = "To";																		// '\0' Is Assumed , Size Is given As 3, although string has only 2 characters
	char shubh_chArray_05[] = "REAL TIME RENDERING BATCH OF 2020-21";									// '\0' Is Assumed , Size Is given As 40, although string has only 39 characters

	char shubh_chArray_WithoutNullTerminator[] = {'H', 'E', 'L', 'L', 'O'};

	// Code
	printf("\n\n");

	printf("Size Of chArray_01 : %lu\n\n", sizeof(shubh_chArray_01));
	printf("Size Of chArray_02 : %lu\n\n", sizeof(shubh_chArray_02));
	printf("Size Of chArray_03 : %lu\n\n", sizeof(shubh_chArray_03));
	printf("Size Of chArray_04 : %lu\n\n", sizeof(shubh_chArray_04));
	printf("Size Of chArray_05 : %lu\n\n", sizeof(shubh_chArray_05));

	printf("\n\n");

	printf("The Strings Are : \n\n");
	printf("shubh_chArray_01 : %s\n\n", shubh_chArray_01);
	printf("shubh_chArray_02 : %s\n\n", shubh_chArray_02);
	printf("shubh_chArray_03 : %s\n\n", shubh_chArray_03);
	printf("shubh_chArray_04 : %s\n\n", shubh_chArray_04);
	printf("shubh_chArray_05 : %s\n\n", shubh_chArray_05);

	printf("\n\n");
	printf("Size Of shubh_chArray_WithoutNullTerminator : %lu\n\n", sizeof(shubh_chArray_WithoutNullTerminator));
	printf("shubh_chArray_WithoutNullTerminator : %s\n\n", shubh_chArray_WithoutNullTerminator);	// Will Display Garbage Value At The End Of String Due To Absence Of '\0'

	return 0;
}
