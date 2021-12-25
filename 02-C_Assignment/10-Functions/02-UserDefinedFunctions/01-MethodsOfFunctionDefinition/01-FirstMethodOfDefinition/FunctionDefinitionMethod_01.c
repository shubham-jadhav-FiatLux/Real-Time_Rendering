// Function Definition Method 1

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype / Declaration / Signature
	void MyAddition();

	// Code
	MyAddition();	// Function Call

	return 0;
}

//****** USER DEFINED FUNCTION : METHOD OF DEFINITION 1 ******
//****** NO RETURN VALUE , NO PARAMETERS ******

void MyAddition(void) {

	// Variable Declarations
	int shubh_a, shubh_b, shubh_sum;		// Local Variables To MyAddition()

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &shubh_b);

	shubh_sum = shubh_a + shubh_b;

	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", shubh_a, shubh_b, shubh_sum);
}
