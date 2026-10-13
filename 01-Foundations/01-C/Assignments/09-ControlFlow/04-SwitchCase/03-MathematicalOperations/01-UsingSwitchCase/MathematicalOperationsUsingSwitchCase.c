// Program For Mathematical Operations Using Switch Case

#include <stdio.h>
#include <conio.h>

int main(void) {

	// variable Declarations
	int shubh_a, shubh_b;
	int shubh_result;
	char shubh_option, shubh_option_division;

	// Code
	printf("\n\n");

	printf("Enter Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("Enter Value For 'B' : ");
	scanf("%d", &shubh_b);

	printf("Enter Option In Character     : \n\n");
	printf("'A' or 'a' For Addition       : \n");
	printf("'S' or 's' For Subtraction    : \n");
	printf("'M' or 'm' For Multiplication : \n");
	printf("'D' or 'd' For Division       : \n");

	printf("Enter Option : ");
	shubh_option = getch();

	printf("\n\n");

	switch (shubh_option) {
	
	// Fall Through Condition For 'A' And 'a'
	case 'A':
	case 'a':
		shubh_result = shubh_a + shubh_b;
		printf("Addition Of A = %d And B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
		break;



	// Fall Through Condition For 'S' And 's'
	case 'S':
	case 's':
		if (shubh_a >= shubh_b) {
			shubh_result = shubh_a - shubh_b;
			printf("Subtraction Of B = %d From A = %d Gives Result %d !!\n\n", shubh_b, shubh_a, shubh_result);
		}
		else {

			shubh_result = shubh_b - shubh_a;
			printf("Subtraction Of A = %d From B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
		}
		break;



	// Fall Through Condition For 'M' And 'm'
	case 'M':
	case 'm':
		shubh_result = shubh_a * shubh_b;
		printf("Multiplication Of A = %d And B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
		break;



	// Fall Through Condition For 'D' And 'd'
	case 'D':
	case 'd':
		printf("Enter Option In Character     : \n\n");
		printf("'Q' or 'q' or '/' For Quotient Upon Division   : \n");
		printf("'R' or 'r' or '%%' For Remainder Upon Division   : \n");

		
		printf("Enter Option : ");
		shubh_option_division = getch();

		printf("\n\n");

		switch (shubh_option) {

		// Fall Through Condition For 'Q' And 'q' And '/'
		case 'Q':
		case 'q':
		case '/':
			if (shubh_a >= shubh_b) {
				shubh_result = shubh_a / shubh_b;
				printf("Division Of A = %d By B = %d Gives Quotient %d !!\n\n", shubh_a, shubh_b, shubh_result);
			}
			else {
			
				shubh_result = shubh_b / shubh_a;
				printf("Division Of B = %d By A = %d Gives Quotient %d !!\n\n", shubh_b, shubh_a, shubh_result);
			}
			break;


		// Fall Through Condition For 'R' And 'r' And '%'
		case 'R':
		case 'r':
		case '%':
			if (shubh_a >= shubh_b) {
				shubh_result = shubh_a % shubh_b;
				printf("Division Of A = %d By B = %d Gives Remainder %d !!\n\n", shubh_a, shubh_b, shubh_result);
			}
			else {

				shubh_result = shubh_b % shubh_a;
				printf("Division Of B = %d By A = %d Gives Remainder %d !!\n\n", shubh_b, shubh_a, shubh_result);
			}
			break;

		default :
			printf("Invalid Character %c Entered For Division !! Please Try Again...\n\n", shubh_option_division);
			break;
		}
		break;


	default:
		printf("Invalid Character %c Entered !! Please Try Again...\n\n", shubh_option);
		break;
	}

	printf("Switch Case Block Complete !!\n\n");
	return 0;
}