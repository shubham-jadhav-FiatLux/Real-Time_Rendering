// Program For Macro Function

#include <stdio.h>

#define MAX_NUMBER(shubh_a, shubh_b) ((shubh_a > shubh_b) ? shubh_a : shubh_b)

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iNum_01;
	int shubh_iNum_02;
	int shubh_iResult;

	float shubh_fNum_01;
	float shubh_fNum_02;
	float shubh_fResult;

	// Code
	
	// *** COMPARING INTEGER VALUES ***
	printf("\n\n");
	printf("Enter An Integer Number : \n\n");
	scanf("%d", &shubh_iNum_01);

	printf("\n\n");
	printf("Enter Another Integer Number : \n\n");
	scanf("%d", &shubh_iNum_02);

	shubh_iResult = MAX_NUMBER(shubh_iNum_01, shubh_iNum_02);
	printf("\n\n");
	printf("Result Of Macro Function MAX_NUMBER() = %d\n", shubh_iResult);
	printf("\n\n");


	// *** COMPARING INTEGER VALUES ***
	printf("\n\n");
	printf("Enter An Floating Point Number : \n\n");
	scanf("%f", &shubh_fNum_01);

	printf("\n\n");
	printf("Enter Another Floating Point Number : \n\n");
	scanf("%f", &shubh_fNum_02);

	shubh_fResult = MAX_NUMBER(shubh_fNum_01, shubh_fNum_02);
	printf("\n\n");
	printf("Result Of Macro Function MAX_NUMBER() = %f\n", shubh_fResult);
	printf("\n\n");

	return 0;
}
