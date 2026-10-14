#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};			

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct MyData data;

	// Code
	// Assigning data values to the data members of 'struct MyData'
	data.shubh_i = 30;
	data.shubh_f = 11.45f;
	data.shubh_d = 1.2995;
	data.shubh_c = 'A';

	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", data.shubh_i);
	printf("shubh_f = %f\n", data.shubh_f);
	printf("shubh_d = %lf\n", data.shubh_d);
	printf("shubh_c = %c\n", data.shubh_c);

	printf("\n\n");
	printf(" ADDRESSES OF DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i occupies addresses from %p\n", &data.shubh_i);
	printf("shubh_f occupies addresses from %p\n", &data.shubh_f);
	printf("shubh_d occupies addresses from %p\n", &data.shubh_d);
	printf("shubh_c occupies addresses from %p\n", &data.shubh_c);

	printf("Starting address of 'struct MyData' variable 'data' = %p\n\n", &data);
	
	return 0;
}
