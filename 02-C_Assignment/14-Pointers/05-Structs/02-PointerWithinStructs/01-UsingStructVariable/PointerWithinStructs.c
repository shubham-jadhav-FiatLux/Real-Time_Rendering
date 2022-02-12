#include <stdio.h>

// Defining Struct
struct MyData {

	int* ptr_i;
	int shubh_i;
	
	float* ptr_f;
	float shubh_f;

	double* ptr_d;
	double shubh_d;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct MyData Data;

	// Code
	Data.shubh_i = 9;
	Data.ptr_i = &Data.shubh_i;
	
	Data.shubh_f = 11.45f;
	Data.ptr_f = &Data.shubh_f;

	Data.shubh_d = 30.2995;
	Data.ptr_d = &Data.shubh_d;

	printf("\n\n");
	
	printf("shubh_i = %d\n", *(Data.ptr_i));
	printf("Address of shubh_i = %p\n", Data.ptr_i);

	printf("shubh_f = %f\n", *(Data.ptr_f));
	printf("Address of shubh_f = %p\n", Data.ptr_f);

	printf("shubh_d = %lf\n", *(Data.ptr_d));
	printf("Address of shubh_d = %p\n", Data.ptr_f);

	return 0;
}
