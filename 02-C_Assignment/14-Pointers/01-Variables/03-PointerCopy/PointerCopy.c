#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_num;
	int* shubh_ptr = NULL;
	int* shubh_copyPtr = NULL;

	// Code
	shubh_num = 5;
	shubh_ptr = &shubh_num;

	printf("\n\n");
	printf("******* Before copyPtr = shubh_ptr ********\n\n");
	printf("num                 = %d\n\n", shubh_num);
	printf("&num                = %p\n\n", &shubh_num);
	printf("*(&num)				= %d\n\n", *(&shubh_num));
	printf("ptr				    = %p\n\n", shubh_ptr);
	printf("*ptr				= %d\n\n", *shubh_ptr);

	shubh_copyPtr = shubh_ptr;

	printf("\n\n");
	printf("******* Before copyPtr = shubh_ptr ********\n\n");
	printf("num                 = %d\n\n", shubh_num);
	printf("&num                = %p\n\n", &shubh_num);
	printf("*(&num)				= %d\n\n", *(&shubh_num));
	printf("ptr				    = %p\n\n", shubh_ptr);
	printf("*ptr				= %d\n\n", *shubh_ptr);
	printf("copyPtr				    = %p\n\n", shubh_copyPtr);
	printf("*copyPtr				= %d\n\n", *shubh_copyPtr);

	return 0;
}

