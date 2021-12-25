// Function Definition Method 2

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype / Declaration / Signature
	int MyAddition(void);

	// Variable Declarations;
	int shubh_result;

	// Code
	shubh_result = MyAddition();	// Function Call

	printf("\n\n");
	printf("Sum = %d\n\n", shubh_result);

	return 0;
}

//****** USER DEFINED FUNCTION : METHOD OF DEFINITION 2 ******
//****** VALID (int) RETURN VALUE, NO PARAMETERS ******

int MyAddition() {

	// Variable Declarations
	int shubh_a, shubh_b, shubh_sum;

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &shubh_b);

	shubh_sum = shubh_a + shubh_b;

	return shubh_sum;
}
