// Program For User Controlled Infinite 'do-while' Loop Without Using If-Else

#include <stdio.h>

int main(void) {

	// Variable Declarations
	char shubh_option, shubh_ch = '\0';

	// Code
	printf("\n\n");

	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite Do-While Loop : \n\n");

	do {
	
		do {
		
			printf("\n");
			printf("Inside Loop...\n");
			//fflush(stdin);
			shubh_ch = getch();			// Flow Control Waits For Character Input...
		} while (shubh_ch != 'Q' && shubh_ch != 'q');

		printf("\n\n");
		printf("Exitting User Controlled Infinite Loop : \n\n");

		printf("DO YOU WANT TO BEGIN USER CONTROLLED INFINITE LOOP AGAIN ?...(Y/y Yes, Any Other Key - No ) : ");
		shubh_option = getch();

	} while (shubh_option == 'Y' || shubh_option == 'y');

	return 0;
}


/*
---> In Inner do-while we have to spacify '&&' logical operator.
---> In Outer do-while we have to specify '||' logical operator.
*/