#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void MyAlloc(int** ptr, unsigned int numberOfElements);

	// Variable Declarations
	int* shubh_piArray = NULL;
	unsigned int shubh_numElements;
	int shubh_i;

	// Code
	printf("\n\n");
	printf("How many elements you want in integer array ? \n\n");
	scanf("%u", &shubh_numElements);

	printf("\n\n");
	MyAlloc(&shubh_piArray, shubh_numElements);

	printf("Enter %u Elements to fill up your integer array : \n\n", shubh_numElements);
	for (shubh_i = 0; shubh_i < shubh_numElements; shubh_i++)
		scanf("%u", &shubh_piArray[shubh_i]);

	printf("The %u Elements entered by yuo in the integer array : \n\n", shubh_numElements);
	for (shubh_i = 0; shubh_i < shubh_numElements; shubh_i++)
		printf("%u\n", shubh_piArray[shubh_i]);

	printf("\n\n");
	if (shubh_piArray) {
	
		free(shubh_piArray);
		shubh_piArray = NULL;
		printf("Memory allocated has now been successfully freed !!\n\n");
	}

	return 0;
}

void MyAlloc(int** ptr, unsigned int numberOfElements) {

	// Code
	*ptr = (int*)malloc(numberOfElements * sizeof(int));
	if (*ptr == NULL) {
	
		printf("Could not allocate memory !!. Exiting ...\n\n");
		exit(0);
	}
	printf("MyAlloc() haas successfully allocated %lu bytes for integer array !!\n\n",(numberOfElements * sizeof(int)));
}
