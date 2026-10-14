#include <stdio.h>

struct MyStruct {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

struct MyUnion {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct MyStruct s;
	struct MyUnion u;

	// Code
	printf("\n\n");
	printf("Size of MyStruct = %lu\n", sizeof(s));

	printf("\n\n");
	printf("Size of MyUnion = %lu\n", sizeof(u));

	return 0;
}
