#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_num = 5;
	const int* shubh_ptr = NULL;
	
	// Code
	shubh_ptr = &shubh_num;

	printf("\n\n");
	printf("Current value of 'num' = %d\n", shubh_num);
	printf("Current value of 'ptr' (Address of num) = %p\n", shubh_ptr);
	
	shubh_num++;
	printf("\n\n");
	printf("After num++, value of 'num' = %d\n", shubh_num);

	shubh_ptr++;
	printf("After ptr++, value of 'ptr' = %p\n", shubh_ptr);
	printf("Value of this new 'ptr' = %d\n\n", *shubh_ptr);

	return 0;
}

