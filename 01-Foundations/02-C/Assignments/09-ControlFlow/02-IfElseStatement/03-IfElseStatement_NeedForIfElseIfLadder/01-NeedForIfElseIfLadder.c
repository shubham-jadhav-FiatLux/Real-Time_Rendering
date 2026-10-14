// Program For If Else Statement Without if-else Ladder

#include <stdio.h>

int main(void) {

	// Variable Declaration 
	int num;

	// Code
	printf("\n\n");

	printf("Enter Value For Num : ");
	scanf("%d", &num);

	if (num < 0) {	// 'if' - 01
	
		printf("Num = %d IS Less Than 0 (NEGATIVE) !!\n\n", num);
	}
	else {	// 'else' - 01
	
		if ((num > 0) && (num <= 100)) {
		
			printf("Num = %d Is Between 0 ANd 100 !!\n\n", num);
		}
		else {	// 'else' - 02

			if ((num > 100) && (num <= 200)) {

				printf("Num = %d Is Between 100 ANd 200 !!\n\n", num);
			}
			else {	// 'else' - 03

				if ((num > 200) && (num <= 300)) {

					printf("Num = %d Is Between 200 ANd 300 !!\n\n", num);
				}
				else {	// 'else' - 04

					if ((num > 300) && (num <= 400)) {

						printf("Num = %d Is Between 300 ANd 400 !!\n\n", num);
					}
					else {	// 'else' - 05

						if ((num > 400) && (num <= 500)) {

							printf("Num = %d Is Between 400 ANd 500 !!\n\n", num);
						}
						else {	// 'else' - 06

							printf("Num = %d Is Greater Than 500 !!\n\n", num);
							
						}	//closing brace of 'else' - 06

					}	//closing brace of 'else' - 05

				}	//closing brace of 'else' - 04

			}	//closing brace of 'else' - 03

		}	//closing brace of 'else' - 02

	}	//closing brace of 'else' - 01

	return 0;
}
