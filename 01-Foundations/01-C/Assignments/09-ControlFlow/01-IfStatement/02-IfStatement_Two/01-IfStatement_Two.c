// Program For Checking Eligibility For Voting Using If Statement

#include <stdio.h>

int main(void) {

	// Variable  Declaration
	int age;

	// Code
	printf("\n\n");
	printf("Enter Age : ");
	scanf("%d", &age);

	if (age >= 18) {
	
		printf("you Are Eligible For Voting !!!\n\n");
	}

	return 0;
}
