#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {


	struct MyData dataOne = { 35, 3.9f, 1.23765, 'A' };			// Inline initialization of struct variable of type 'struct MyData' globally ...


	struct MyData dataTwo = { 'P', 6.2f, 12.199523, 68};			// Inline initialization of struct variable of type 'struct MyData' globally ...


	struct MyData dataThree = { 36, 'G' };			// Inline initialization of struct variable of type 'struct MyData' globally ...


	struct MyData dataFour = { 79 };			// Inline initialization of struct variable of type 'struct MyData' globally ...

	// Code
	// Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData dataOne' ARE : \n\n");
	printf("shubh_i = %d\n", dataOne.shubh_i);
	printf("shubh_f = %f\n", dataOne.shubh_f);
	printf("shubh_d = %lf\n", dataOne.shubh_d);
	printf("shubh_c = %c\n\n", dataOne.shubh_c);

	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData dataTwo' ARE : \n\n");
	printf("shubh_i = %d\n", dataTwo.shubh_i);
	printf("shubh_f = %f\n", dataTwo.shubh_f);
	printf("shubh_d = %lf\n", dataTwo.shubh_d);
	printf("shubh_c = %c\n\n", dataTwo.shubh_c);

	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData dataThree' ARE : \n\n");
	printf("shubh_i = %d\n", dataThree.shubh_i);
	printf("shubh_f = %f\n", dataThree.shubh_f);
	printf("shubh_d = %lf\n", dataThree.shubh_d);
	printf("shubh_c = %c\n\n", dataThree.shubh_c);

	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData Four' ARE : \n\n");
	printf("shubh_i = %d\n", dataFour.shubh_i);
	printf("shubh_f = %f\n", dataFour.shubh_f);
	printf("shubh_d = %lf\n", dataFour.shubh_d);
	printf("shubh_c = %c\n\n", dataFour.shubh_c);

	return 0;
}
