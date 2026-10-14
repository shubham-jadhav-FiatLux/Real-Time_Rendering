// Program For User Controlled Infinite 'for' Loop 

#include <stdio.h>

int main(void) {

	// Variable Declarations
	char shubh_option, shubh_ch = '\0';

	// Code
	printf("\n\n");

	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");
	printf("Enter 'Y' or 'y' To Initiate User Controlled Infinite Loop : \n\n");

	shubh_option = getch();
	if (shubh_option == 'Y' || shubh_option == 'y') {
	
		for (;;) {		// Infinite Loop
		
			printf("In Loop...\n");
			shubh_ch = getch();
			if (shubh_ch == 'Q' || shubh_ch == 'q')
				break;		//User Controlled Exitting From Infinite Loop
		}
	}

	printf("\n\n");
	printf("Exitting User Controlled Infinite Loop...\n\n");

	return 0;
}
