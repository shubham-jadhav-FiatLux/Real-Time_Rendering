// My variable code

#include <stdio.h>

int main(void) {

	// Variable declaration
	char sj_c;
	short sj_s;
	int sj_i;
	long sj_l;
	long long sj_ll;
	float sj_f;
	double sj_d;
	long double sj_ld;
	
	//void sj_v;		--------> Error but 1 Byte allocated

	// Variable initialization
	sj_c = 'S';
	sj_s = 256;
	sj_i = 65536;
	sj_l = 65536L;
	sj_ll = 65536LL;
	sj_f = 3.14f;
	sj_d = 3.14;
	sj_ld = 3.1456789l;

	// Code
	printf("\n\n");

	printf("char = %c\n", sj_c);
	printf("short = %d\n", sj_s);
	printf("int = %d\n", sj_i);
	printf("long = %ld\n", sj_l);
	printf("long long = %lld\n", sj_ll);
	printf("float = %f\n", sj_f);
	printf("double = %lf\n", sj_d);
	printf("long double = %llf\n", sj_ld);

	printf("\n\n");

	return 0;
}
