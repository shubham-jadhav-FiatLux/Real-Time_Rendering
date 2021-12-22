// Program for Use Of 'continue' In Nested Loop

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	printf("Outer Loop Prints Odd Numbers Between 1 And 10. \n\n");
	printf("Inner Loop Prints Even Numbers Between 1 And 10 For Every Odd Number Printed By Outer Loop.\n\n");

	//Condition For A Number To Be Even Number => Division Of NUmber By 2 Leaves NO Remainder (remainder = 0) 
	//Condition For A Number To Be Odd Number => Division Of NUmber By 2 Leaves Remainder (remainder = 1)

	for (shubh_i = 1; shubh_i <= 10; shubh_i++) {
	
		if (shubh_i % 2 != 0) {		// If Number 'shubh_i' Is Odd
		
			printf("shubh_i = %d\n", shubh_i);
			printf("-----------------\n");

			for (shubh_j = 1; shubh_j <= 10; shubh_j++) {
			
				if (shubh_j % 2 == 0) {		// IF Number 'shuh_j' Is Even...
				
					printf("\tshubh_j = %d\n", shubh_j);
				}
				else {					// If Number 'shubh_j' Is Odd...
				
					continue;
				}
			}
			printf("\n\n");
		}
		else {						//If Number 'shubh_i' Is Evevn...
		
			continue;
		}
	}

	printf("\n\n");

	return 0;
}
