// Program For Floating Point Variable Iteration In 'while' Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	float shubh_f;
	float shubh_f_num = 1.7f;		// Simply Change This Value Only To Get Different Output...

	// Code
	printf("\n\n");

	printf("Printing Numbers From %f To %f : \n\n", shubh_f_num, (shubh_f_num * 10.0f));

	shubh_f = shubh_f_num;

	while (shubh_f <= (shubh_f_num * 10.0f)) {
	
		printf("\t%f\n", shubh_f);
		shubh_f += shubh_f_num;
	}

	printf("\n\n");

	return 0;
}
