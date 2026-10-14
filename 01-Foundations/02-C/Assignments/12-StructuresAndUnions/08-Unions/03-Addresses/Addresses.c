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
	printf("MEMBERS OF Struct ARE : \n\n");

	s.shubh_i = 6;
	s.shubh_f = 1.2f;
	s.shubh_d = 8.333333;
	s.shubh_c = 'S';

	printf("s.shubh_i = %d\n", s.shubh_i);
	printf("s.shubh_f = %f\n", s.shubh_f);
	printf("s.shubh_d = %lf\n", s.shubh_d);
	printf("s.shubh_c = %c\n\n", s.shubh_c);

	printf("ADDRESSES OF MEMBERS OF Struct ARE : \n\n");
	printf("s.shubh_i = %p\n", &s.shubh_i);
	printf("s.shubh_f = %p\n", &s.shubh_f);
	printf("s.shubh_d = %p\n", &s.shubh_d);
	printf("s.shubh_c = %p\n\n", &s.shubh_c);

	printf("MyStruct s = %p\n\n", &s);

	// MyUnion 
	printf("\n\n");
	printf("MEMBERS OF Union ARE : \n\n");

	u.shubh_i = 3;
	printf("u.shubh_i = %d\n", u.shubh_i);

	u.shubh_f = 4.5f;
	printf("u.shubh_f = %f\n", u.shubh_f);

	u.shubh_d = 5.12764;
	printf("u.shubh_d = %lf\n", u.shubh_d);

	u.shubh_c = 'A';
	printf("u.shubh_c = %c\n\n", u.shubh_c);

	printf("ADDRESSES OF MEMBERS OF Union ARE : \n\n");
	printf("u.shubh_i = %p\n", &u.shubh_i);
	printf("u.shubh_f = %p\n", &u.shubh_f);
	printf("u.shubh_d = %p\n", &u.shubh_d);
	printf("u.shubh_c = %p\n\n", &u.shubh_c);

	printf("MyUnion u = %p\n\n", &u);


	printf("\n\n");
	printf("Size of MyStruct = %lu\n", sizeof(s));

	printf("\n\n");
	printf("Size of MyUnion = %lu\n", sizeof(u));

	return 0;
}
