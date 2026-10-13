#include <stdio.h>
#include <stdlib.h>		// For malloc() and free()

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int *shubh_ptr_iArray = NULL;
	unsigned int shubh_intArrayLength = 0;
	int shubh_i;

	// Code
	printf("\n\n");
	printf("Enter the number of elements you want in your integer array : ");
	scanf("%d", &shubh_intArrayLength);

	shubh_ptr_iArray = (int*)malloc(sizeof(int) * shubh_intArrayLength);
	if (shubh_ptr_iArray == NULL) {
	
		printf("\n\n");
		printf("Memory allocation for integer array has failed !! Exiting ...\n\n");
		exit(0);
	}
	else {
	
		printf("\n\n");
		printf("Memory allocation for integer array has succeeded !!\n\n");
		printf("Memory addresses from %p to %p have been allocated to integer array !!\n\n", shubh_ptr_iArray, (shubh_ptr_iArray + (shubh_intArrayLength - 1)));
	}

	printf("\n\n");
	printf("Enter %d elements for the integer array : \n\n", shubh_intArrayLength);
	for (shubh_i = 0; shubh_i < shubh_intArrayLength; shubh_i++)
		scanf("%d", (shubh_ptr_iArray + shubh_i));

	printf("\n\n");
	printf("The integer array entered by you, consisting of %d elements  : \n\n", shubh_intArrayLength);
	for (shubh_i = 0; shubh_i < shubh_intArrayLength; shubh_i++)
		printf("shubh_ptr_iArray[%d] = %d \t\t at address &shubh_ptr_iArray[%d] : %p\n", shubh_i, shubh_ptr_iArray[shubh_i], shubh_i, &shubh_ptr_iArray[shubh_i]);

	printf("\n\n");
	for (shubh_i = 0; shubh_i < shubh_intArrayLength; shubh_i++)
		printf("*(shubh_ptr_iArray + %d) = %d \t\t at address (shubh_ptr_iArray + %d) : %p\n", shubh_i, *(shubh_ptr_iArray + shubh_i), shubh_i, (shubh_ptr_iArray + shubh_i));

	if (shubh_ptr_iArray) {
	
		free(shubh_ptr_iArray);
		shubh_ptr_iArray = NULL;

		printf("\n\n");
		printf("Memory allocated for integer array has been successfully freed !!\n\n");
	}

	return 0;
}
