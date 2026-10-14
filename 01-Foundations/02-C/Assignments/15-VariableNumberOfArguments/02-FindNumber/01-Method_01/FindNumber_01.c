#include <stdio.h>
#include <stdarg.h>

#define NUM_TO_BE_FOUND 3
#define NUM_ELEMENTS 10

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void FindNumber(int, int, ...);

	// Code
	printf("\n\n");

	FindNumber(NUM_TO_BE_FOUND, NUM_ELEMENTS, 3, 5, 9, 2, 3, 6, 9, 3, 1, 3);

	return 0;
}

void FindNumber(int num_to_be_found, int num, ...) {

	// Variable Declarations
	int shubh_count = 0;
	int shubh_n;
	va_list Numbers_list;

	// Code
	va_start(Numbers_list, num);

	while (num) {

		shubh_n = va_arg(Numbers_list, int);
		if (shubh_n == num_to_be_found)
			shubh_count++;
		num--;
	}

	if (shubh_count == 0)
		printf("Number %d could not be found !!\n\n", num_to_be_found);
	else
		printf("Number %d found %d times !!\n\n", num_to_be_found, shubh_count);
	
	va_end(Numbers_list);
}

