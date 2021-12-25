// Funtion Defintion Method 4

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Funtion Prototype / Declaration / Signature
	int MyAddition(int, int);

	// Variable Declarations
	int shubh_a, shubh_b, shubh_result;		// Local Variables To main()

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &shubh_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &shubh_b);

	shubh_result = MyAddition(shubh_a, shubh_b);
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", shubh_a, shubh_b, shubh_result);

	return 0;
}

//****** USER DEFINED FUNCTION : METHOD OF DEFINITION 3 ******
//****** VALID (int) RETURN VALUE, VALID PARAMETERS (int, int)******

int MyAddition(int shubh_a, int shubh_b) {

	// Variable Declarations
	int shubh_sum;

	// Code
	shubh_sum = shubh_a + shubh_b;
	return shubh_sum;
}
