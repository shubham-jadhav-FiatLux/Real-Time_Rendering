#include <stdio.h>

struct MyNumber {

	int shubh_num;
	int shubh_numTable[10];
};

struct NumTables {

	struct MyNumber n;
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct NumTables tables[10];
	int shubh_i, shubh_j;

	// Code
	for (shubh_i = 0; shubh_i < 10; shubh_i++)
		tables[shubh_i].n.shubh_num = (shubh_i + 1);

	for (shubh_i = 0; shubh_i < 10; shubh_i++) {

		printf("\n\n");
		printf("Table Of %d : \n\n", tables[shubh_i].n.shubh_num);

		for (shubh_j = 0; shubh_j < 10; shubh_j++) {

			tables[shubh_i].n.shubh_numTable[shubh_j] = tables[shubh_i].n.shubh_num * (shubh_j + 1);
			printf("%d * %d = %d\n", tables[shubh_i].n.shubh_num, (shubh_j + 1), tables[shubh_i].n.shubh_numTable[shubh_j]);
		}
	}
	
	return 0;
}
