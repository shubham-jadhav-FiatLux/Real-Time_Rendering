#include <stdio.h>

struct MyUnion {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {


	struct MyUnion u1, u2;

	// Code
	printf("\n\n");
	printf("MEMBERS OF Union u1 ARE : \n\n");

	u1.shubh_i = 6;
	u1.shubh_f = 1.2f;
	u1.shubh_d = 8.333333;
	u1.shubh_c = 'S';

	printf("u1.shubh_i = %d\n", u1.shubh_i);
	printf("u1.shubh_f = %f\n", u1.shubh_f);
	printf("u1.shubh_d = %lf\n", u1.shubh_d);
	printf("u1.shubh_c = %c\n\n", u1.shubh_c);

	printf("ADDRESSES OF MEMBERS OF Union u1 ARE : \n\n");
	printf("u1.shubh_i = %p\n", &u1.shubh_i);
	printf("u1.shubh_f = %p\n", &u1.shubh_f);
	printf("u1.shubh_d = %p\n", &u1.shubh_d);
	printf("u1.shubh_c = %p\n\n", &u1.shubh_c);

	printf("MyUnion u1 = %p\n\n", &u1);

	// MyUnion u2
	printf("\n\n");
	printf("MEMBERS OF Union u2 ARE : \n\n");

	u2.shubh_i = 3;
	printf("u2.shubh_i = %d\n", u2.shubh_i);

	u2.shubh_f = 4.5f;
	printf("u2.shubh_f = %f\n", u2.shubh_f);

	u2.shubh_d = 5.12764;
	printf("u2.shubh_d = %lf\n", u2.shubh_d);

	u2.shubh_c = 'A';
	printf("u2.shubh_c = %c\n\n", u2.shubh_c);

	printf("ADDRESSES OF MEMBERS OF Union u2 ARE : \n\n");
	printf("u2.shubh_i = %p\n", &u2.shubh_i);
	printf("u2.shubh_f = %p\n", &u2.shubh_f);
	printf("u2.shubh_d = %p\n", &u2.shubh_d);
	printf("u2.shubh_c = %p\n\n", &u2.shubh_c);

	printf("MyUnion u2 = %p\n\n", &u2);

	return 0;
}
