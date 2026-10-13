#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray[10];
	int shubh_i;

	// Code
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		shubh_iArray[shubh_i] = (shubh_i + 1) * 3;

	printf("\n\n");
	printf("Elements of the Integer array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_iArray[%d] = %d\n", shubh_i, shubh_iArray[shubh_i]);

	printf("\n\n");
	printf("Elements of the Integer array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_iArray[%d] = %d \t\t Address = %p\n", shubh_i, shubh_iArray[shubh_i], &shubh_iArray[shubh_i]);
	
	printf("\n\n");

	return 0;
}
