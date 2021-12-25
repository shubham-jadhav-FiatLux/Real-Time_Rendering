// Program For Recursive Functon

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Dclarations
	unsigned int shubh_num;

	// Function Prototypes
	void recursive(unsigned int);

	// Code
	printf("\n\n");
	printf("Enter Any Number : \n\n");
	scanf("%u", &shubh_num);

	printf("\n\n");
	printf("Output Of Recursive Function : \n\n");

	recursive(shubh_num);

	printf("\n\n");

	return 0;
}

void recursive(unsigned int shubh_n) {

	// Code
	printf("shubh_n = %u\n", shubh_n);

	if (shubh_n > 0) {
	
		recursive(shubh_n - 1);
	}
}
