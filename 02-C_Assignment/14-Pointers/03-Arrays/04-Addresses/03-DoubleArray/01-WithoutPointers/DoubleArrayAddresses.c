#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	double shubh_dArray[10];
	int shubh_i;

	// Code
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		shubh_dArray[shubh_i] = (float)(shubh_i + 1) * 1.333333f;

	printf("\n\n");
	printf("Elements of the Double array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_dArray[%d] = %lf\n", shubh_i, shubh_dArray[shubh_i]);

	printf("\n\n");
	printf("Elements of the Double array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_dArray[%d] = %lf \t\t Address = %p\n", shubh_i, shubh_dArray[shubh_i], &shubh_dArray[shubh_i]);

	printf("\n\n");

	return 0;
}
