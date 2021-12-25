// Program For Global Variables

#include <stdio.h>

// *** GLOBAL SCOPE ***

/* If Not Initialized By Us, Global Variables Are Initialized To Their Zero Value
*  (with respect to their data types i.e.: 0 For int, 0.0 For float and double etc.) By Default.
* 
*  But Stil, For Good Programming Discipline, We Shall Explicitly Initialize Our Global Variable With 0.
*/

int shubh_global_count = 0;

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototypes
	void change_count_one(void);
	void change_count_two(void);
	void change_count_three(void);
		
	// Code
	printf("\n\n");

	printf("main() : Value Of global_count = %d\n", shubh_global_count);

	change_count_one();
	change_count_two();
	change_count_three();

	printf("\n\n");

	return 0;
}

// *** GLOBAL SPACE ***

void change_count_one(void) {

	// Code
	shubh_global_count = 100;
	printf("change_count_one() : Value Of global_count = %d\n", shubh_global_count);
}

void change_count_two(void) {

	// Code
	shubh_global_count += 1;
	printf("change_count_two() : Value Of global_count = %d\n", shubh_global_count);
}

void change_count_three(void) {

	// Code
	shubh_global_count += 10;
	printf("change_count_one() : Value Of global_count = %d\n", shubh_global_count);
}
