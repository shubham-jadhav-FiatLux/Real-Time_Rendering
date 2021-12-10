// Program For If Else Statement

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int age;

	// Code
	printf("\n\n");
	printf("Enter Age : ");
	scanf("%d", &age);
	printf("\n\n");

	if (age >= 18) {
	
		printf("Entering if-Block ...\n\n");
		printf("You Are Eligible For Voting !!\n\n");
	}
	else {
	
		printf("Entering else-Block ...\n\n");
		printf("You Are NOT Eligible For Voting !!\n\n");
	}
	printf("Bye !! \n\n");

	return 0;
}
