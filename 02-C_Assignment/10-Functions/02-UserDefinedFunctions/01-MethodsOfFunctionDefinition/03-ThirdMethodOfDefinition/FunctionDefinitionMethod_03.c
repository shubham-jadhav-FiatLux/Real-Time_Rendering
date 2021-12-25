// Function Definition Method 3

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype / Declaration / Signature
	void MyAddition(int, int);

	// Variable Declarations
	int shubh_a, shubh_b;		// Local Variables To main()

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &shubh_b);

	MyAddition(shubh_a, shubh_b);		// Function Call

	return 0;
}

//****** USER DEFINED FUNCTION : METHOD OF DEFINITION 3 ******
//****** NO RETURN VALUE, VALID PARAMETERS (int, int)******

void MyAddition(int shubh_p, int shubh_q) {

	// Variable Declarations
	int shubh_sum;		// Local Variables To MyAddition()

	// Code
	shubh_sum = shubh_p + shubh_q;
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", shubh_p, shubh_q, shubh_sum);
}
