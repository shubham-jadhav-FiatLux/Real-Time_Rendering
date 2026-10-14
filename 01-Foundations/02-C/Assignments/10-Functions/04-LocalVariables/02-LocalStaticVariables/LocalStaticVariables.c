// Program For Ordinary Local Variables

#include <stdio.h>

//*** GLOBAL SCOPE ***

int main(int argc, char* argv[], char* envp[]) {

	// *** LOCAL SCOPE OF main() BEGINS ***

	// Variable Declarations
	int shubh_a = 5;

	// Function Prototypes
	void change_count(void);

	// Code
	printf("\n\n");
	printf("A = %d\n\n", shubh_a);

	change_count();

	//Since, 'shubh_local_count' Ia An Ordinary Local Variable Of change_count() , 
	//Will NOT Retain Its Value From Preveous call To change_count().

	// So 'shubh_local_count' Is 1
	// shubh_local_count = shubh_local_count + 1 = 1 + 1 = 2

	change_count();


	//Since, 'shubh_local_count' Ia An Ordinary Local Variable Of change_count() , 
	//Will NOT Retain Its Value From Preveous call To change_count().

	// So 'shubh_local_count' Is 2
	// shubh_local_count = shubh_local_count + 1 = 2 + 1 = 3

	change_count();

	return 0;

	// *** LOCAL SCOPE OF main() ENDS ***
}

void change_count(void) {

	// *** LOCAL SCOPE OF change_count() BEGINS ***

	// Variable Declarations
	// 'shubh_local_count' Is A Local Static Variable. It Is Local To change_count() Only.
	// It Will Retain Its Value Between Calls To change_count()
	static int shubh_local_count = 0;

	//Code
	shubh_local_count += 1;

	printf("Local Count = %d\n\n", shubh_local_count);

	// *** LOCAL SCOPE OF change_count() ENDS ***
}
