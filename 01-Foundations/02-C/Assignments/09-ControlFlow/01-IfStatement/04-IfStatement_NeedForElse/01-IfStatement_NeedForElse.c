// Program To Check Eligibility For Voting Without Using Else

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int age;

	// code
	printf("\n\n");
	printf("Enter age : ");
	scanf("%d", &age);

	if (age >= 18) {
	
		printf("You Are Eligible For Voting !!\n\n");
	}

	printf("You Are NOT Eligible For Voting !!\n\n");

	return 0;
}
