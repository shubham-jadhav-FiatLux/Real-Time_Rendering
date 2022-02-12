#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_num;
	int* shubh_ptr = NULL;
	int ans;

	// Code
	shubh_num = 5;
	shubh_ptr = &shubh_num;

	printf("\n\n");
	printf("Answer of (ptr + 10) = %p\n", (shubh_ptr + 10));

	printf("Answer of *(ptr + 10) = %d\n", *(shubh_ptr + 10));

	printf("Answer of (*ptr + 10) = %d\n", (*shubh_ptr + 10));

	++*shubh_ptr;
	printf("Answer of ++*ptr = %d\n", *shubh_ptr);

	*shubh_ptr++;
	printf("Answer of *ptr++ = %d\n", *shubh_ptr);

	shubh_ptr = &shubh_num;
	(*shubh_ptr)++;
	printf("Answer of (*ptr)++ = %d\n\n", *shubh_ptr);

	return 0;
}

