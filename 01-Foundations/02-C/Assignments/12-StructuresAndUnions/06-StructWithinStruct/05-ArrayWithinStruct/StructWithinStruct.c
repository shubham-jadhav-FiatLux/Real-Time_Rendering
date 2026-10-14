#include <stdio.h>

struct MyNumber {

	int shubh_num;
	int shubh_numTable[10];
};

struct NumTables {

	struct MyNumber a;
	struct MyNumber b;
	struct MyNumber c;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct NumTables tables;
	int shubh_i;

	// Code
	tables.a.shubh_num = 2;
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		tables.a.shubh_numTable[shubh_i] = tables.a.shubh_num * (shubh_i + 1);
	printf("\n\n");
	printf("Table Of %d : \n\n", tables.a.shubh_num);
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("%d * %d = %d\n", tables.a.shubh_num, (shubh_i + 1), tables.a.shubh_numTable[shubh_i]);


	tables.a.shubh_num = 3;
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		tables.b.shubh_numTable[shubh_i] = tables.b.shubh_num * (shubh_i + 1);
	printf("\n\n");
	printf("Table Of %d : \n\n", tables.b.shubh_num);
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("%d * %d = %d\n", tables.b.shubh_num, (shubh_i + 1), tables.b.shubh_numTable[shubh_i]);


	tables.a.shubh_num = 4;
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		tables.c.shubh_numTable[shubh_i] = tables.c.shubh_num * (shubh_i + 1);
	printf("\n\n");
	printf("Table Of %d : \n\n", tables.c.shubh_num);
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		printf("%d * %d = %d\n", tables.c.shubh_num, (shubh_i + 1), tables.c.shubh_numTable[shubh_i]);

	return 0;
}
