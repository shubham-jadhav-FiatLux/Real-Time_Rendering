#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Defining Struct
	struct MyData {

		int shubh_i;
		float shubh_f;
		double shubh_d;
	}data;			// Declaring a single struct variable of type 'struct MyData' globally ...

	// Variable Declarations
	int shubh_iSize;
	int shubh_fSize;
	int shubh_dSize;
	int shubh_structMyDataSize;

	// Code
	// Assigning data values to the data members of 'struct MyData'
	data.shubh_i = 30;
	data.shubh_f = 11.45f;
	data.shubh_d = 1.2995;

	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", data.shubh_i);
	printf("shubh_f = %f\n", data.shubh_f);
	printf("shubh_d = %lf\n", data.shubh_d);

	// Calculating Sizes (In Bytes) of the data members of 'struct MyData'
	shubh_iSize = sizeof(data.shubh_i);
	shubh_fSize = sizeof(data.shubh_f);
	shubh_dSize = sizeof(data.shubh_d);

	// Displaying Sizes (In Bytes) of the data members of 'struct MyData'
	printf("\n\n");
	printf("SIZES (In Bytes) OF DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("Size of 'shubh_i' = %d bytes\n", shubh_iSize);
	printf("Size of 'shubh_f' = %d bytes\n", shubh_fSize);
	printf("Size of 'shubh_d' = %d bytes\n", shubh_dSize);

	// Calculating Sizes (In Bytes) of the entire 'struct MyData'
	shubh_structMyDataSize = sizeof(struct MyData);

	// Displaying Sizes (In Bytes) of the entire 'struct MyData'
	printf("\n\n");
	printf("Size of 'struct MyData' : %d bytes\n\n", shubh_structMyDataSize);

	return 0;
}
