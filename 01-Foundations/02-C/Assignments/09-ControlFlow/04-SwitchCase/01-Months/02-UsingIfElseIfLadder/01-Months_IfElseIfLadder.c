// program to get effect of switch case using if-else-if ladder

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int sj_num_months;

	// Code
	printf("\n\n");

	printf("Enter Number Of Month (1 To 12) : ");
	scanf("%d", &sj_num_months);

	printf("\n\n");

	// IF-ELSE-IF LADDER BEGINS FROM HERE
	if (sj_num_months == 1)			// Like 'Case 1'
		printf("Month Number %d Is JANUARY !!\n\n", sj_num_months);

	else if (sj_num_months == 2)		//Like 'Case 2'
		printf("Month Number %d Is FEBRUARY !!\n\n", sj_num_months);

	else if (sj_num_months == 3)		//Like 'Case 3'
		printf("Month Number %d Is MARCH !!\n\n", sj_num_months);

	else if (sj_num_months == 4)		//Like 'Case 4'
		printf("Month Number %d Is APRIL !!\n\n", sj_num_months);

	else if (sj_num_months == 5)		//Like 'Case 5'
		printf("Month Number %d Is MAY !!\n\n", sj_num_months);

	else if (sj_num_months == 6)		//Like 'Case 6'
		printf("Month Number %d Is JUNE !!\n\n", sj_num_months);

	else if (sj_num_months == 7)		//Like 'Case 7'
		printf("Month Number %d Is JULY !!\n\n", sj_num_months);

	else if (sj_num_months == 8)		//Like 'Case 8'
		printf("Month Number %d Is AUGUST !!\n\n", sj_num_months);

	else if (sj_num_months == 9)		//Like 'Case 9'
		printf("Month Number %d Is SEPTEMBER !!\n\n", sj_num_months);

	else if (sj_num_months == 10)	//Like 'Case 10'
		printf("Month Number %d Is OCTOBER !!\n\n", sj_num_months);

	else if (sj_num_months == 11)	//Like 'Case 11'
		printf("Month Number %d Is NOVEMBER !!\n\n", sj_num_months);

	else if (sj_num_months == 12)	//Like 'Case 12'
		printf("Month Number %d Is DECEMBER !!\n\n", sj_num_months);

	else		// Like 'default'...'default' Is Optional In switch-case, So Is 'else' In The if-else-if Ladder...
		printf("Invalid Month Number %d Is Entered !!! Please Try Again...\n\n", sj_num_months);


	printf("If-Else If-Else Ladder Complete !!\n\n");
	
	return 0;
}
