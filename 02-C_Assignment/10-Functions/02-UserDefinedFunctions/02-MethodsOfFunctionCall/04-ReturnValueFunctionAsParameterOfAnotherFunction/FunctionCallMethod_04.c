// Function Calling Method 4

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototypes
	int MyAddition(int, int);

	// Variable Declarations
	int shubh_r;
	int shubh_num1, shubh_num2;
	int shubh_num3, shubh_num4;

	// Code
	shubh_num1 = 10;
	shubh_num2 = 20;
	shubh_num3 = 30;
	shubh_num4 = 40;

	shubh_r = MyAddition(MyAddition(shubh_num1, shubh_num2), MyAddition(shubh_num3, shubh_num4));
	//  Return Value Of MyAddition() Is Sent A Parameter To Another Call To MyAddition()
	
	printf("\n\n");
	printf("%d + %d + %d + %d = %d\n\n", shubh_num1, shubh_num2, shubh_num3, shubh_num4, shubh_r);
	
	return 0;
}

//*** Function Definition Of MyAddition() ***
int MyAddition(int shubh_one, int shubh_two) {

	// Variable Declarations
	int shubh_sum;

	// Code
	shubh_sum = shubh_one + shubh_two;

	return shubh_sum;
}