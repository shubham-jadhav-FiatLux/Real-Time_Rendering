// Program For 'for' Loop Using Floating Point Variable Iterator

#include <stdio.h>

int main(void) {

	// Variable Declarations
	float shubh_f;
	float shubh_f_num = 1.7f;		//Simply Change This Value Only To Get Different Outputs...

	// Code
	printf("\n\n");

	printf("Printing Numbers %f To %f : \n\n", shubh_f_num, (shubh_f_num * 10.0f));

	for (shubh_f = shubh_f_num; shubh_f <= (shubh_f_num * 10.0f); shubh_f += shubh_f_num) {
	
		printf("\t%2.6f\n", shubh_f);
	}

	printf("\n\n");

	return 0;
}
