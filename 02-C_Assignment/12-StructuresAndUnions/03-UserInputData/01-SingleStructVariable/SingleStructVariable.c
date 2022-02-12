#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declaration
	struct MyData data;			// Inline initialization of struct variable of type 'struct MyData' globally ...

	// Code
	printf("\n\n");
	printf("Enter Integer value for data member 'i' of 'struct MyData' : \n");
	scanf("%d", &data.shubh_i);

	printf("Enter Floating-Point value for data member 'f' of 'struct MyData' : \n");
	scanf("%f", &data.shubh_f);

	printf("Enter Double value for data member 'd' of 'struct MyData' : \n");
	scanf("%lf", &data.shubh_d);

	printf("Enter Charater value for data member 'c' of 'struct MyData' : \n");
	data.shubh_c = getch();
	
	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("shubh_i = %d\n", data.shubh_i);
	printf("shubh_f = %f\n", data.shubh_f);
	printf("shubh_d = %lf\n", data.shubh_d);
	printf("shubh_c = %c\n\n", data.shubh_c);

	return 0;
}
