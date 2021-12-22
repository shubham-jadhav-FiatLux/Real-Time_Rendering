// Program For Break Statement

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i;
	char shubh_ch;

	// Code
	printf("\n\n");

	printf("Printing Numbers From 1 To 100 For Every User Input , Exitting The Loop When User Enters Characters 'Q' or 'q' : \n\n");
	printf("Enter Character 'Q' or 'q' To Exit Loop : \n\n");

	for (shubh_i = 1; shubh_i <= 100; shubh_i++) {
	
		printf("\t%d\n", shubh_i);
		shubh_ch = getch();

		if (shubh_ch == 'Q' || shubh_ch == 'q')
			break;
	}

	printf("\n\nExitting Loop...\n\n");

	return 0;
}
