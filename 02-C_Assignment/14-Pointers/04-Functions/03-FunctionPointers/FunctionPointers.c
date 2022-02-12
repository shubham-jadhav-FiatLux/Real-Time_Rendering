#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Function Declarations
	int AddIntegers(int, int);
	int SubtractIntegers(int, int);
	float AddFloats(float, float);

	// Variable Declarations
	typedef int (*AddIntsFnPtr)(int, int);
	AddIntsFnPtr ptrAddTwoIntegers = NULL;
	AddIntsFnPtr ptrFunc = NULL;

	typedef float (*AddFloatsFnPtr)(float, float);
	AddFloatsFnPtr ptrAddTwoFloats = NULL;

	int shubh_iAnswer = 0;
	float shubh_fAnswer = 0.0f;

	// Code
	ptrAddTwoIntegers = AddIntegers;
	shubh_iAnswer = ptrAddTwoIntegers(9, 30);
	printf("\n\n");
	printf("Sum Of Integers = %d\n\n", shubh_iAnswer);


	ptrFunc = SubtractIntegers;
	shubh_iAnswer = ptrFunc(9, 30);
	printf("\n\n");
	printf("Subtraction Of Integers = %d\n\n", shubh_iAnswer);


	ptrAddTwoFloats = AddFloats;
	shubh_fAnswer = ptrAddTwoFloats(11.45f, 8.2f);
	printf("\n\n");
	printf("Sum Of Floating-Point Numbers = %f\n\n", shubh_fAnswer);

	return 0;
}

int AddIntegers(int a, int b) {

	// Variable Declarations
	int c;

	// Code
	c = a + b;
	return c;
}

int SubtractIntegers(int a, int b) {

	// Variable Declarations
	int c;

	// Code
	if (a > b)
		c = a - b;
	else
		c = b - a;

	return c;
}

float AddFloats(float f_num1, float f_num2) {

	// Variable Declarations
	float shubh_ans;

	// Code
	shubh_ans = f_num1 + f_num2;

	return shubh_ans;
}