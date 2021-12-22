// Program For User Controlled Infinite 'while' Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	char shubh_option, shubh_ch = '\0';

	// Code
	printf("\n\n");

	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite While Loop : \n\n");
	printf("Enter 'Y' or 'y' To Initiate User Controlled Infinite Loop : \n\n");

	shubh_option = getch();
	if (shubh_option == 'Y' || shubh_option == 'y') {

		while (1) {		// Infinite Loop

			printf("Inside Loop...\n");
			shubh_ch = getch();
			if (shubh_ch == 'Q' || shubh_ch == 'q')
				break;		// User Controlled Exitting From Infinite Loop
		}

		printf("\n\n");
		printf("Exitting User Controlled Infinite While Loop...\n\n");
	}
	else
		printf("You Must Press 'Y' or 'y' To Initiate The User Controlled Infinite Loop ...Please Try Again ...\n\n");

	return 0;
}
