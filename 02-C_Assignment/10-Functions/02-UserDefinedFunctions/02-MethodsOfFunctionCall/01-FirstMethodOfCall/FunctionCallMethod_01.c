// Function Calling Method 1

#include <stdio.h>

//***** USER DEFINIED FUNCTIONS : METHOD OF CALLING FUNCTION 1 ******
// Calling All FUnctions Directly In main() 

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype OR Declaration OR Signature
	void MyAddition(void);
	int MySubtraction(void);
	void MyMultiplication(int, int);
	int MyDivision(int, int);

	// Variable Declarations
	int shubh_result_subtraction;
	int shubh_a_multiplication, shubh_b_multiplication;
	int shubh_a_division, shubh_b_division, shubh_result_division;

	// Code

	//*** Addition ***
	MyAddition();		// Function Call

	//*** Subtraction ***
	shubh_result_subtraction = MySubtraction();
	printf("\n\n");
	printf("Subtraction Yields Result = %d\n\n", shubh_result_subtraction);

	//*** Multiplication ***
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Multiplication : ");
	scanf("%d", &shubh_a_multiplication);

	printf("\n\n");
	printf("Enter Integer Value For 'B' For Multiplication : ");
	scanf("%d", &shubh_b_multiplication);

	MyMultiplication(shubh_a_multiplication, shubh_b_multiplication);

	//*** Division ***
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Division : ");
	scanf("%d", &shubh_a_division);

	printf("\n\n");
	printf("Enter Integer Value For 'B' For Division : ");
	scanf("%d", &shubh_b_division);

	shubh_result_division = MyDivision(shubh_a_division, shubh_b_division);
	printf("\n\n");
	printf("Division Of %d And %d Gives = %d (Quotient)\n\n", shubh_a_division, shubh_b_division, shubh_result_division);
	
	printf("\n\n");

	return 0;
}

//*** Function Definition Of MyAddition() ***
void MyAddition(void) {

	// Variable Declarations : Local Variables To MyAddition()
	int shubh_one, shubh_two, shubh_sum;

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Addition : ");
	scanf("%d", &shubh_one);

	printf("\n\n");
	printf("Enter Integer Value For 'B' For Addition : ");
	scanf("%d", &shubh_two);

	shubh_sum = shubh_one + shubh_two;

	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", shubh_one, shubh_two, shubh_sum);
}

//*** Function Definition Of MyAddition() ***
int MySubtraction(void) {

	// Variable Declarations : Local Variables To MyAddition()
	int shubh_one, shubh_two, shubh_subtraction;

	// Code
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Subtraction : ");
	scanf("%d", &shubh_one);

	printf("\n\n");
	printf("Enter Integer Value For 'B' For Subtraction : ");
	scanf("%d", &shubh_two);

	shubh_subtraction = shubh_one - shubh_two;
	return shubh_subtraction;
}

//*** Function Definition Of MyMultiplication(int, int) ***
void MyMultiplication(int shubh_one, int shubh_two) {

	// Variable Declarations : Local Variables To MyAddition()
	int shubh_multiplication;

	// Code
	
	shubh_multiplication = shubh_one * shubh_two;

	printf("\n\n");
	printf("Multiplication Of %d And %d = %d\n\n", shubh_one, shubh_two, shubh_multiplication);
}

//*** Function Definition Of MyDivision(int, int) ***
int MyDivision(int shubh_one, int shubh_two) {

	// Variable Declarations : Local Variables To MyAddition()
	int shubh_division_quotient;

	// Code
	
	if(shubh_one > shubh_two)
		shubh_division_quotient = shubh_one / shubh_two;
	else
		shubh_division_quotient = shubh_two / shubh_one;

	return shubh_division_quotient;
}
