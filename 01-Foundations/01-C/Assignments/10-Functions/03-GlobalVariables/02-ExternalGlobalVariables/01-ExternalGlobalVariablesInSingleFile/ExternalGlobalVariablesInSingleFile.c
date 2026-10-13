// Program For External Global Variable In Single File

#include <stdio.h>

int main(int argc, char* argvp[], char* envp[]) {

	// Function Declarations
	void change_count(void);

	// variable Declarations
	extern int shubh_global_count;

	// Code
	printf("\n\n");
	printf("Value Of global_count Before change_count() = %d\n\n", shubh_global_count);
	change_count();
	printf("Value Of global_count After change_count() = %d\n\n", shubh_global_count);

	return 0;
}

//*** GLOBAL SCOPE ***

/* global_count Is A Global Variable.
*  Since, It Is Declaraed Before change_count(), It Can Be Accessed And used As Any Ordinary Global Variable In change_count()
*  Since, It Is Declaraed After change_count(), It must Be First Re-Declared In main() As An External Global variable By Means Of The 'extern' Keyword And The Type Of The Variable.
*  Once This Is Done, It can Be Used As An Ordinary Global Variable In main() As Well.
*/

int shubh_global_count = 0;

void change_count(void) {

	// Code
	shubh_global_count = 5;
	printf("Value Of global_count In change_count() = %d\n\n", shubh_global_count);
}
