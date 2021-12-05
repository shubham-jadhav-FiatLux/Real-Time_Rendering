// Program For Logical Operators

#include <stdio.h>

int main(void) {

	// Variable Declaration
	int a;
	int b;
	int c;
	int result;

	// Code
	printf("\n\n");
	printf("Enter First Integer : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Second Integer : ");
	scanf("%d", &b);

	printf("\n\n");
	printf("Enter Third Integer : ");
	scanf("%d", &c);

	printf("\n\n");
	printf("If Answer = 0, It Is 'FALSE'\n");
	printf("If Answer = 1, It Is 'TRUE'\n\n");

	result = (a <= b) && (b != c);
	printf("LOGICAL AND (&&) : Answer Is TRUE (1) If And Only If BOTH Conditions Are True. The Answer Is FALSE (0), If Any One Or BOTH Conditions Are False.\n\n");
	printf("a = %d Is Less Than Or Equal To b = %d AND b = %d Is Not Equal To c = %d               \t Answer = %d\n\n", a, b, b, c, result);

	printf("\n\n");

	result = (b >= a) && (a == c);
	printf("LOGICAL OR (||) : Answer Is FALSE (0) If And Only If BOTH Conditions Are False. The Answer Is TRUE (1), If Any One Or BOTH Conditions Are True.\n\n");
	printf("Either b = %d Is Greater Than Or Equal To a = %d OR a = %d Is Equal To c = %d          \t Answer = %d\n\n", b, a, a, c, result);

	printf("\n\n");

	result = !a;
	printf("a = %d And Using Logical NOT (!) Operator On 'a' Gives Result = %d\n\n", a, result);

	result = !b;
	printf("b = %d And Using Logical NOT (!) Operator On 'b' Gives Result = %d\n\n", b, result);

	result = !c;
	printf("c = %d And Using Logical NOT (!) Operator On 'c' Gives Result = %d\n\n\n", c, result);

	result = (!(a <= b) && !(b != c));
	printf("Using Logical NOT (!) On (a <= b) And Also On (b != c) And Then Anding Them Afterwards Gives Result = %d\n", result);
	printf("\n\n");

	result = (!(b >= a) && !(a == c));
	printf("Using Logical NOT (!) On Entire Logical Expression (b >= a) || (a == c) Gives Result = %d\n", result);
	printf("\n\n");

	return 0;
}
