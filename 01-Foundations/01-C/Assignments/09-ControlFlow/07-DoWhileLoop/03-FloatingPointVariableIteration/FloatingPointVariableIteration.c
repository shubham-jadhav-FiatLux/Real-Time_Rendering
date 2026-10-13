// Program For Floating point Variable Iteration In 'do-while' Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	float shubh_f;
	float shubh_f_num = 1.7f;		// Simply Change This Value Only To Get Different Output

	// Code
	printf("\n\n");

	printf("Printing Numbers Form %f To %f : \n\n", shubh_f_num, (shubh_f_num * 10.f));
	
	shubh_f = shubh_f_num;

	do {
	
		printf("\n");
		printf("\t%f", shubh_f);
		shubh_f += shubh_f_num;
	} while (shubh_f <= (shubh_f_num * 10.f));

	printf("\n\n");

	return 0;
}
