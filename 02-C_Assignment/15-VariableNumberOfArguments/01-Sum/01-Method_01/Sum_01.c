#include <stdio.h>
#include <stdarg.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	int CalculateSum(int, ...);

	// Variable Declarations
	int shubh_answer;

	// Code
	printf("\n\n");

	shubh_answer = CalculateSum(5, 10, 20, 30, 40, 50);
	printf("Answer = %d\n\n", shubh_answer);

	shubh_answer = CalculateSum(10, 1, 1, 1, 1, 1, 1, 1, 1);
	printf("Answer = %d\n\n", shubh_answer);

	shubh_answer = CalculateSum(0);
	printf("Answer = %d\n\n", shubh_answer);

	return 0;
}

int CalculateSum(int num, ...) {

	// Variable Declarations
	int shubh_sumTotal = 0;
	int shubh_n;

	va_list Numbers_list;

	// Code
	va_start(Numbers_list, num);
	while (num) {
	
		shubh_n = va_arg(Numbers_list, int);
		shubh_sumTotal += shubh_n;
		num--;
	}

	va_end(Numbers_list);
	return(shubh_sumTotal);
}
