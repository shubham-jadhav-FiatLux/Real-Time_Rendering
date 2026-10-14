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

	if (shubh_option == 'A' || shubh_option == 'a') {

		shubh_result = shubh_a + shubh_b;
		printf("Addition Of A = %d And B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
	}
	else if (shubh_option == 'S' || shubh_option == 's') {
		if (shubh_a >= shubh_b) {
			shubh_result = shubh_a - shubh_b;
			printf("Subtraction Of B = %d From A = %d Gives Result %d !!\n\n", shubh_b, shubh_a, shubh_result);
		}
		else {

			shubh_result = shubh_b - shubh_a;
			printf("Subtraction Of A = %d From B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
		}
	}
	else if (shubh_option == 'M' || shubh_option == 'm') {
	
		shubh_result = shubh_a * shubh_b;
		printf("Multiplication Of A = %d And B = %d Gives Result %d !!\n\n", shubh_a, shubh_b, shubh_result);
	}
	else if (shubh_option == 'D' || shubh_option == 'd') {

		printf("Enter Option In Character     : \n\n");
		printf("'Q' or 'q' or '/' For Quotient Upon Division   : \n");
		printf("'R' or 'r' or '%%' For Remainder Upon Division   : \n");


		printf("Enter Option : ");
		shubh_option_division = getch();

		printf("\n\n");

		if (shubh_option_division == 'Q' || shubh_option_division == 'q' || shubh_option_division == '/') {

				if (shubh_a >= shubh_b) {
					shubh_result = shubh_a / shubh_b;
					printf("Division Of A = %d By B = %d Gives Quotient %d !!\n\n", shubh_a, shubh_b, shubh_result);
				}
				else {

					shubh_result = shubh_b / shubh_a;
					printf("Division Of B = %d By A = %d Gives Quotient %d !!\n\n", shubh_b, shubh_a, shubh_result);
				}
		}
		else if (shubh_option_division == 'R' || shubh_option_division == 'r' || shubh_option_division == '%') {

				if (shubh_a >= shubh_b) {
					shubh_result = shubh_a % shubh_b;
					printf("Division Of A = %d By B = %d Gives Remainder %d !!\n\n", shubh_a, shubh_b, shubh_result);
				}
				else {

					shubh_result = shubh_b % shubh_a;
					printf("Division Of B = %d By A = %d Gives Remainder %d !!\n\n", shubh_b, shubh_a, shubh_result);
				}
		}
		else
			printf("Invalid Character %c Entered For Division !! Please Try Again...\n\n", shubh_option_division);

	}
	else
		printf("Invalid Character %c Entered !! Please Try Again...\n\n", shubh_option);

	printf("If - Else - IF Ladder Complete !!\n\n");

	return 0;
}