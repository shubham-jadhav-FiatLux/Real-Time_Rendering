#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	float shubh_fArray[10];
	float* shubh_ptr_fArray = NULL;
	int shubh_i;

	// Code
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		shubh_fArray[shubh_i] = (float)(shubh_i + 1) * 1.5f;

	shubh_ptr_fArray = shubh_fArray;

	printf("\n\n");
	printf("Elements of the Float array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_fArray[%d] = %f\n", shubh_i, *(shubh_ptr_fArray + shubh_i));

	printf("\n\n");
	printf("Elements of the Float array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_fArray[%d] = %f \t\t Address = %p\n", shubh_i, *(shubh_fArray + shubh_i), (shubh_fArray + shubh_i));

	printf("\n\n");

	return 0;
}
