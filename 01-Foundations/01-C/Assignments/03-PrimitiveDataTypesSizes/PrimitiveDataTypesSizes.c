// Program for sizes of DataTypes

#include <stdio.h>

int main(void) {

	// Code
	printf("\n\n");

	printf("size of 'int'            = %ld Bytes\n", sizeof(int));
	printf("size of 'unsigned int'   = %ld Bytes\n", sizeof(unsigned int));
	printf("size of 'long'           = %ld Bytes\n", sizeof(long));
	printf("size of 'long long'      = %ld Bytes\n", sizeof(long long));
	
	printf("size of 'float'          = %ld Bytes\n", sizeof(float));
	printf("size of 'double'         = %ld Bytes\n", sizeof(double));
	printf("size of 'long double'    = %ld Bytes\n", sizeof(long double));

	printf("\n\n");

	return 0;
}
