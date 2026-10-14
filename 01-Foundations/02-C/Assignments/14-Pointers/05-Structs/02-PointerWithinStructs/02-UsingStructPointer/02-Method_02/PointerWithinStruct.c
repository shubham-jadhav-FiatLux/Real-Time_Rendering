#include <stdio.h>

// Defining Struct
struct MyData {

	int* ptr_i;
	int shubh_i;

	float* ptr_f;
	float shubh_f;

	double* ptr_d;
	double shubh_d;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
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

	pData->shubh_i = 9;
	pData->ptr_i = &pData->shubh_i;

	pData->shubh_f = 11.45f;
	pData->ptr_f = &pData->shubh_f;

	pData->shubh_d = 30.2995;
	pData->ptr_d = &pData->shubh_d;

	printf("\n\n");

	printf("shubh_i = %d\n", *(pData->ptr_i));
	printf("Address of shubh_i = %p\n", pData->ptr_i);

	printf("shubh_f = %f\n", *(pData->ptr_f));
	printf("Address of shubh_f = %p\n", pData->ptr_f);

	printf("shubh_d = %lf\n", *(pData->ptr_d));
	printf("Address of shubh_d = %p\n", pData->ptr_f);

	if (pData) {

		free(pData);
		pData = NULL;
		printf("Memory allocated to 'struct MyData' has been successfully freed !!\n\n");
	}

	return 0;
}
