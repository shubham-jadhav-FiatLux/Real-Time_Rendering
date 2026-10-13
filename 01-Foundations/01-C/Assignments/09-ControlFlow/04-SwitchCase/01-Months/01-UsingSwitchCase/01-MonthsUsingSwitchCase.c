// program for months using switch case

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int num_month;

	// Code
	printf("\n\n");

	printf("Enter Number Of Month (1 To 12) : ");
	scanf("%d", &num_month);

	printf("\n\n");

	switch (num_month) {
	
	case 1 :	//like 'if'
		printf("Month Number %d Is JANUARY !!\n\n", num_month);
		break;

	case 2 :	//Like 'else-if'
		printf("Month Number %d Is FEBRUARY!!\n\n", num_month);
		break;

	case 3 :	//Like 'else-if'
		printf("Month Number %d Is MARCH!!\n\n", num_month);
		break;

	case 4 :	//Like 'else-if'
		printf("Month Number %d Is APRIL !!\n\n", num_month);
		break;

	case 5 :	//Like 'else-if'
		printf("Month Number %d Is MAY !!\n\n", num_month);
		break;

	case 6 :	//Like 'else-if'
		printf("Month Number %d Is JUNE !!\n\n", num_month);
		break;

	case 7 :	//Like 'else-if'
		printf("Month Number %d Is JULY !!\n\n", num_month);
		break;

	case 8 :	//Like 'else-if'
		printf("Month Number %d Is AUGUST !!\n\n", num_month);
		break;

	case 9 :	//Like 'else-if'
		printf("Month Number %d Is SEPTEMBER !!\n\n", num_month);
		break;

	case 10 :	//Like 'else-if'
		printf("Month Number %d Is OCTOBER !!\n\n", num_month);
		break;

	case 11 :	//Like 'else-if'
		printf("Month Number %d Is NOVEMBER !!\n\n", num_month);
		break;

	case 12 :	//Like 'else-if'
		printf("Month Number %d Is DECEMBER !!\n\n", num_month);
		break;

	default :	//Like Ending OPTIONAL 'else' ... Just Like Terminating 'else' Is Optional In if-else if-else Ladder, So Is The 'Default' Case Optional In Switch-case

		printf("Invalid Month Number %d Entered !!. Please Try Again...\n\n", num_month);
		break;
	}

	printf("Switch Case Block Complete !!\n");

	return 0;
}

