#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_cArray[10];
	char* shubh_ptr_cArray = NULL;
	int shubh_i;

	// Code
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		shubh_cArray[shubh_i] = (char)(shubh_i + 65);

	shubh_ptr_cArray = shubh_cArray;

	printf("\n\n");
	printf("Elements of the Character array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_cArray[%d] = %c\n", shubh_i, *(shubh_ptr_cArray + shubh_i));

	printf("\n\n");
	printf("Elements of the Character array : \n\n");
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("shubh_cArray[%d] = %c \t\t Address = %p\n", shubh_i, *(shubh_cArray + shubh_i), (shubh_cArray + shubh_i));

	printf("\n\n");

	return 0;
}
