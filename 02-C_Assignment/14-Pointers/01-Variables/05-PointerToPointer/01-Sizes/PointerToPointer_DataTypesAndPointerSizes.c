#include <stdio.h>

struct Employee {

	char shubh_name[100];
	int shubh_age;
	float shubh_salary;
	char shubh_sex;
	char shubh_maritalStatus;
};

int main(int argc, char* argv[], char* envp[]) {

	// Code
	printf("\n\n");
	printf("Sizes of data types and pointer to those data types are : \n\n");
	printf("Size of (int)      : %d \t\t\t Size of pointer to int (int*)      : %d \t\t\t Size of pointer to pointer to int (int**)      : %d \n\n", sizeof(int), sizeof(int*), sizeof(int**));
	printf("Size of (float)      : %d \t\t\t Size of pointer to float (float*)      : %d \t\t\t Size of pointer to pointer to float (float**)      : %d \n\n", sizeof(float), sizeof(float*), sizeof(float**));
	printf("Size of (double)      : %d \t\t\t Size of pointer to double (double*)      : %d \t\t\t Size of pointer to pointer to double (double**)      : %d \n\n", sizeof(double), sizeof(double*), sizeof(double**));
	printf("Size of (char)      : %d \t\t\t Size of pointer to char (char*)      : %d \t\t\t Size of pointer to pointer to char (char**)      : %d \n\n", sizeof(char), sizeof(char*), sizeof(char**));
	printf("Size of (struct Employee)      : %d \t\t\t Size of pointer to struct Employee (struct Employee*)      : %d \t\t\t Size of pointer to pointer to struct Employee (struct Employee**)      : %d \n\n", sizeof(struct Employee), sizeof(struct Employee*), sizeof(struct Employee**));

	return 0;
}
