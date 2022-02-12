#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iSize;
	int shubh_fSize;
	int shubh_dSize;
	int shubh_structMyDataSize;
	int shubh_PtrTostructMyDataSize;

	struct MyData* pData = NULL;

	// Code
	printf("\n\n");
	pData = (struct MyData*)malloc(sizeof(struct MyData));
	if (pData == NULL) {

		printf("Failed to allocate memory to 'struct MyData' !!. Exiting ...\n\n");
		exit(0);
	}
	else
		printf("Successfully allocated memory to 'struct MyData' !!\n\n");

	// Assigning data values to the data members of 'struct MyData'
	pData->shubh_i = 30;
	pData->shubh_f = 11.45f;
	pData->shubh_d = 1.2995;

	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", pData->shubh_i);
	printf("shubh_f = %f\n", pData->shubh_f);
	printf("shubh_d = %lf\n", pData->shubh_d);

	// Calculating Sizes (In Bytes) of the data members of 'struct MyData'
	shubh_iSize = sizeof(pData->shubh_i);
	shubh_fSize = sizeof(pData->shubh_f);
	shubh_dSize = sizeof(pData->shubh_d);

	// Displaying Sizes (In Bytes) of the data members of 'struct MyData'
	printf("\n\n");
	printf("SIZES (In Bytes) OF DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("Size of 'shubh_i' = %d bytes\n", shubh_iSize);
	printf("Size of 'shubh_f' = %d bytes\n", shubh_fSize);
	printf("Size of 'shubh_d' = %d bytes\n", shubh_dSize);

	// Calculating Sizes (In Bytes) of the entire 'struct MyData'
	shubh_structMyDataSize = sizeof(struct MyData);
	shubh_PtrTostructMyDataSize = sizeof(struct MyData*);

	// Displaying Sizes (In Bytes) of the entire 'struct MyData'
	printf("\n\n");
	printf("Size of 'struct MyData' : %d bytes\n\n", shubh_structMyDataSize);
	printf("Size of pointer to 'struct MyData' : %d bytes\n\n", shubh_PtrTostructMyDataSize);

	if (pData) {

		free(pData);
		pData = NULL;
		printf("Memory allocated to 'struct MyData' has been successfully freed !!\n\n");
	}

	return 0;
}
