#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
};

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void ChangeValues(struct MyData*);

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

	// Assigning data values to the data members of 'struct MyData'
	(*pData).shubh_i = 30;
	(*pData).shubh_f = 11.45f;
	(*pData).shubh_d = 1.2995;

	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", (*pData).shubh_i);
	printf("shubh_f = %f\n", (*pData).shubh_f);
	printf("shubh_d = %lf\n", (*pData).shubh_d);

	ChangeValues(pData);
	
	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", (*pData).shubh_i);
	printf("shubh_f = %f\n", (*pData).shubh_f);
	printf("shubh_d = %lf\n", (*pData).shubh_d);


	if (pData) {

		free(pData);
		pData = NULL;
		printf("Memory allocated to 'struct MyData' has been successfully freed !!\n\n");
	}

	return 0;
}

void ChangeValues(struct MyData* pParam_data) {

	// Code
	pParam_data->shubh_i = 9;
	pParam_data->shubh_f = 8.2f;
	pParam_data->shubh_d = 6.1999;
}
