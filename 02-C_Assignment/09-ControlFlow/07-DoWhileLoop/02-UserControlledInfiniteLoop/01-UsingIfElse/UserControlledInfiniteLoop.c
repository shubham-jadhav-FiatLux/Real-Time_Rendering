// Program For User Controlled Infinite 'do-while' Loop Using If-Else

#include <stdio.h>

int main(void) {

	// Variable Declarations
	char shubh_option, shubh_ch = '\0';

	// Code
	printf("\n\n");

	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite Do-While Loop : \n\n");
	printf("Enter 'Y' or 'y' To Initiate User Controlled Infinite Loop : \n\n");

	shubh_option = getch();
	if (shubh_option == 'Y' || shubh_option == 'y') {

		do {

			printf("Inside Loop...\n");
			shubh_ch = getch();			//Control Flow Waits For Character Input ...
			if (shubh_ch == 'Q' || shubh_ch == 'q')
				break;
		} while (1);		// Infinite Loop

		printf("\n\n");
		printf("Exitting User Controlled Infinite Loop...\n\n");
	}
	else
		printf("You Must Press 'Y' or 'y' To Initiate The User Controlled Infinite Loop...Please Try Again ...\n\n");

	return 0;
}
