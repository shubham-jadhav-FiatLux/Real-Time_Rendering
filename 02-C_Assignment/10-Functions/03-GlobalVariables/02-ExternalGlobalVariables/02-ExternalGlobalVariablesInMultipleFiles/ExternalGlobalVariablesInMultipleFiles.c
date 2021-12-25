// program For External Global Variables In Multiple Files

#include <stdio.h>

//*** GLOBAL SCOPE ***
int shubh_global_count = 0;

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void change_count(void);
	void change_count_one(void);		// Function Defined In File_01.c
	void change_count_two(void);		// Function Defined In File_02.c

	// Code
	printf("\n\n");

	change_count();
	change_count_one();					// Function Defines In File_01.c
	change_count_two();					// Function Defines In File_02.c

	return 0;
}

void change_count(void) {

	// Code
	shubh_global_count += 1;
	printf("Global Count = %d\n", shubh_global_count);
}
