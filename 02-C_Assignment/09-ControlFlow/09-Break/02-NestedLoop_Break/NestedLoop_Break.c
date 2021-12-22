// Program for 'break' Statement

#include <stdio.h>
#include <conio.h>

int main(void) {

	// Variable Declarations
	int shubh_i, shubh_j;

	// Code
	printf("\n\n");

	for (shubh_i = 1; shubh_i <= 20; shubh_i++) {
	
		for (shubh_j = 1; shubh_j <= 20; shubh_j++) {
		
			if (shubh_j > shubh_i) {
			
				break;
			}
			else {
			
				printf("* ");
			}
		}
		printf("\n");
	}
	printf("\n\n");

	return 0;
}