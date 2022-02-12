#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

struct MyData data = { 9, 8.2f, 9.451995, 'P' };			// Inline initialization of struct variable of type 'struct MyData' globally ...

int main(int argc, char* argv[], char* envp[]) {

	// Code
	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", data.shubh_i);
	printf("shubh_f = %f\n", data.shubh_f);
	printf("shubh_d = %lf\n", data.shubh_d);
	printf("shubh_c = %c\n\n", data.shubh_c);

	return 0;
}
