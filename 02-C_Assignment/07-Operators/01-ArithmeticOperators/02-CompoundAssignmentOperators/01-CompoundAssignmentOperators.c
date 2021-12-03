//Program For Compound Assignment Operators

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int a;
	int b;
	int x;

	// Code
	printf("\n\n");
	printf("Enter A Number : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &b);

	printf("\n\n");

	//Since, In All The Following 5 Cases, The Operand On The Left 'a' Is Getting Repeated Immediately On The Right (e.g. : a = a + b),
	//We Are Using Compound Assignment Operators +=, -=, *=, /= and %=

	//Since, 'a' Will Be Assigned The Values Of (a + b) At The Expression (a += b), We Must Save  The Original Value Of 'a' To Another Variable 'x'
	x = a;
	a += b;		// a = a + b
	printf("Addition of a = %d And b = %d Gives %d\n", x, b, a);

	//Value Of 'a' Altered In The Above Expression Is Used Here...
	//Since, 'a' Will Be Assigned The Values Of (a - b) At The Expression (a -= b), We Must Save  The Original Value Of 'a' To Another Variable 'x'
	x = a;
	a -= b;		// a = a - b;
	printf("Subsraction Of a = %d And b = %d Gives %d\n", x, b, a);

	//Value Of 'a' Altered In The Above Expression Is Used Here...
	//Since, 'a' Will Be Assigned The Values Of (a * b) At The Expression (a *= b), We Must Save  The Original Value Of 'a' To Another Variable 'x'
	x = a;
	a *= b;		// a = a * b;
	printf("Multiplication of a = %d And b = %d Gives %d\n", x, b, a);

	//Value Of 'a' Altered In The Above Expression Is Used Here...
	//Since, 'a' Will Be Assigned The Values Of (a / b) At The Expression (a /= b), We Must Save  The Original Value Of 'a' To Another Variable 'x'
	x = a;
	a /= b;		// a = a / b;
	printf("Division Of a = %d And b = %d Gives Quotient %d\n", x, b, a);

	//Value Of 'a' Altered In The Above Expression Is Used Here...
	//Since, 'a' Will Be Assigned The Values Of (a % b) At The Expression (a %= b), We Must Save  The Original Value Of 'a' To Another Variable 'x'
	x = a;
	a %= b;		// a = a % b;
	printf("Division Of a = %d And b = %d Gives Remainder %d\n", x, b, a);

	printf("\n\n");

	return 0;
}
