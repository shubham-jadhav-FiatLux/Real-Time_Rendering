// program for sizes of all DataTypes

#include <stdio.h>

int main(void) {

	// Code
	printf("\n\n");

	printf("size of 'char'               = %ld Bytes\n", sizeof(char));
	

	printf("size of 'short'              = %ld Bytes\n", sizeof(short));
	printf("size of 'unsigned short'     = %ld Bytes\n", sizeof(unsigned short));
	
	printf("size of 'int'                = %ld Bytes\n", sizeof(int));
	printf("size of 'unsigned int'       = %ld Bytes\n", sizeof(unsigned int));

	printf("size of 'long'               = %ld Bytes\n", sizeof(long));
	printf("size of 'long long'          = %ld Bytes\n", sizeof(long long));
	
	
	printf("size of 'float'              = %ld Bytes\n", sizeof(float));
	printf("size of 'double'             = %ld Bytes\n", sizeof(double));
	printf("size of 'long double'        = %ld Bytes\n", sizeof(long double));


	printf("size of 'void'               = %ld Bytes\n", sizeof(void)); // Warning
	// void - Windows = 0
	// void - Linux   = 1

	return 0;
}
