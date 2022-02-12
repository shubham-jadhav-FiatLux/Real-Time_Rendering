#include <stdio.h>
#include <stdlib.h>		// For malloc() and free()

#define INT_SIZE sizeof(int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define CHAR_SIZE sizeof(char)

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int* shubh_ptr_iArray = NULL;
	unsigned int shubh_intArrayLength = 0;
	
	float* shubh_ptr_fArray = NULL;
	unsigned int shubh_floatArrayLength = 0;

	double *shubh_ptr_dArray = NULL;
	unsigned int shubh_doubleArrayLength = 0;

	char* shubh_ptr_cArray = NULL;
	unsigned int shubh_charArrayLength = 0;

	int shubh_i;

	// Code
	// ******Integer Array*******
	printf("\n\n");
	printf("Enter the number of elements you want in your integer array : ");
	scanf("%u", &shubh_intArrayLength);

	shubh_ptr_iArray = (int*)malloc(INT_SIZE * shubh_intArrayLength);
	if (shubh_ptr_iArray == NULL) {

		printf("\n\n");
		printf("Memory allocation for integer array has failed !! Exiting ...\n\n");
		exit(0);
	}
	else {

		printf("\n\n");
		printf("Memory allocation for integer array has succeeded !!\n\n");
	}

	printf("\n\n");
	printf("Enter %d integer elements to fill up the integer array : \n\n", shubh_intArrayLength);
	for (shubh_i = 0; shubh_i < shubh_intArrayLength; shubh_i++)
		scanf("%d", (shubh_ptr_iArray + shubh_i));



	// ******Float Array*******
	printf("\n\n");
	printf("Enter the number of elements you want in your float array : ");
	scanf("%u", &shubh_floatArrayLength);

	shubh_ptr_fArray = (float*)malloc(FLOAT_SIZE * shubh_floatArrayLength);
	if (shubh_ptr_fArray == NULL) {

		printf("\n\n");
		printf("Memory allocation for float array has failed !! Exiting ...\n\n");
		exit(0);
	}
	else {

		printf("\n\n");
		printf("Memory allocation for float array has succeeded !!\n\n");
	}

	printf("\n\n");
	printf("Enter %d floating-point elements to fill up the float array : \n\n", shubh_floatArrayLength);
	for (shubh_i = 0; shubh_i < shubh_floatArrayLength; shubh_i++)
		scanf("%f", (shubh_ptr_fArray + shubh_i));



	// ******Double Array*******
	printf("\n\n");
	printf("Enter the number of elements you want in your double array : ");
	scanf("%u", &shubh_doubleArrayLength);

	shubh_ptr_dArray = (double*)malloc(DOUBLE_SIZE * shubh_doubleArrayLength);
	if (shubh_ptr_dArray == NULL) {

		printf("\n\n");
		printf("Memory allocation for double array has failed !! Exiting ...\n\n");
		exit(0);
	}
	else {

		printf("\n\n");
		printf("Memory allocation for double array has succeeded !!\n\n");
	}

	printf("\n\n");
	printf("Enter %d double elements to fill up the double array : \n\n", shubh_doubleArrayLength);
	for (shubh_i = 0; shubh_i < shubh_doubleArrayLength; shubh_i++)
		scanf("%lf", (shubh_ptr_dArray + shubh_i));



	// ******Character Array*******
	printf("\n\n");
	printf("Enter the number of elements you want in your character array : ");
	scanf("%u", &shubh_charArrayLength);

	shubh_ptr_cArray = (char*)malloc(CHAR_SIZE * shubh_charArrayLength);
	if (shubh_ptr_cArray == NULL) {

		printf("\n\n");
		printf("Memory allocation for Character array has failed !! Exiting ...\n\n");
		exit(0);
	}
	else {

		printf("\n\n");
		printf("Memory allocation for character array has succeeded !!\n\n");
	}

	printf("\n\n");
	printf("Enter %d character elements to fill up the character array : \n\n", shubh_charArrayLength);
	for (shubh_i = 0; shubh_i < shubh_charArrayLength; shubh_i++)
		scanf(" %c", (shubh_ptr_cArray + shubh_i));


	// ************* Display Of Arrays **********
	
	// **** Integer Array *******
	printf("\n\n");
	printf("The integer array entered by you, consisting of %d elements  : \n\n", shubh_intArrayLength);
	for (shubh_i = 0; shubh_i < shubh_intArrayLength; shubh_i++)
		printf("*(shubh_ptr_iArray + %d) = %d \t\t at address (shubh_ptr_iArray + %d) : %p\n", shubh_i, *(shubh_ptr_iArray + shubh_i), shubh_i, (shubh_ptr_iArray + shubh_i));

	// **** Float Array *******
	printf("\n\n");
	printf("The float array entered by you, consisting of %d elements  : \n\n", shubh_floatArrayLength);
	for (shubh_i = 0; shubh_i < shubh_floatArrayLength; shubh_i++)
		printf("*(shubh_ptr_fArray + %d) = %f \t\t at address (shubh_ptr_fArray + %d) : %p\n", shubh_i, *(shubh_ptr_fArray + shubh_i), shubh_i, (shubh_ptr_fArray + shubh_i));

	// **** Double Array *******
	printf("\n\n");
	printf("The double array entered by you, consisting of %d elements  : \n\n", shubh_doubleArrayLength);
	for (shubh_i = 0; shubh_i < shubh_doubleArrayLength; shubh_i++)
		printf("*(shubh_ptr_dArray + %d) = %lf \t\t at address (shubh_ptr_dArray + %d) : %p\n", shubh_i, *(shubh_ptr_dArray + shubh_i), shubh_i, (shubh_ptr_dArray + shubh_i));

	// **** Character Array *******
	printf("\n\n");
	printf("The character array entered by you, consisting of %d elements  : \n\n", shubh_charArrayLength);
	for (shubh_i = 0; shubh_i < shubh_charArrayLength; shubh_i++)
		printf("*(shubh_ptr_cArray + %d) = %c \t\t at address (shubh_ptr_cArray + %d) : %p\n", shubh_i, *(shubh_ptr_cArray + shubh_i), shubh_i, (shubh_ptr_cArray + shubh_i));


	if (shubh_ptr_cArray) {

		free(shubh_ptr_cArray);
		shubh_ptr_cArray = NULL;

		printf("\n\n");
		printf("Memory allocated for Character array has been successfully freed !!\n\n");
	}

	if (shubh_ptr_dArray) {

		free(shubh_ptr_dArray);
		shubh_ptr_dArray = NULL;

		printf("\n\n");
		printf("Memory allocated for double array has been successfully freed !!\n\n");
	}

	if (shubh_ptr_fArray) {

		free(shubh_ptr_fArray);
		shubh_ptr_fArray = NULL;

		printf("\n\n");
		printf("Memory allocated for float array has been successfully freed !!\n\n");
	}

	if (shubh_ptr_iArray) {

		free(shubh_ptr_iArray);
		shubh_ptr_iArray = NULL;

		printf("\n\n");
		printf("Memory allocated for integer array has been successfully freed !!\n\n");
	}

	return 0;
}
