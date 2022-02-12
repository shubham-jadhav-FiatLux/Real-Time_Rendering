#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function prototype
	void MultiplyArrayElementsByNumber(int *, int, int);

	// Variable Declarations
	int* iArray = NULL;
	int num_elements;
	int shubh_i, shubh_num;

	// Code
	printf("\n\n");
	printf("Enter how many elements you want in integer array : ");
	scanf("%d", &num_elements);


	iArray = (int*)malloc(num_elements * sizeof(char));
	if (iArray == NULL) {

		printf("Memory allocation to iArray failed !!. Exiting ...\n\n");
		exit(0);
	}
	printf("\n\n");
	printf("Enter %d elements for the integer array : \n\n", num_elements);
	for (shubh_i = 0; shubh_i < num_elements; shubh_i++)
		scanf("%d", &iArray[shubh_i]);

	// ******* ONE **********

	printf("\n\n");
	printf("Array Before Passing To Function MultiplyArrayElementsByNumber() : \n\n");
	for (shubh_i = 0; shubh_i < num_elements; shubh_i++)
		printf("iArray[%d] = %d\n", shubh_i, iArray[shubh_i]);

	printf("\n\n");
	printf("Enter the number by which you want to multiply each array element : ");
	scanf("%d", &shubh_num);

	MultiplyArrayElementsByNumber(iArray, num_elements, shubh_num);

	printf("\n\n");
	printf("Array returned by Function MultiplyArrayElementsByNumber() : \n\n");
	for (shubh_i = 0; shubh_i < num_elements; shubh_i++)
		printf("iArray[%d] = %d\n", shubh_i, iArray[shubh_i]);


	if (iArray) {

		free(iArray);
		iArray = NULL;
		printf("Memory allocated iArray has been successfully freed !!\n\n");
	}

	return 0;
}

void MultiplyArrayElementsByNumber(int* arr, int iNumElements, int n) {

	// Variable Declarations
	int shubh_i;

	// Code
	for (shubh_i = 0; shubh_i < iNumElements; shubh_i++)
		arr[shubh_i] *= n;
}

