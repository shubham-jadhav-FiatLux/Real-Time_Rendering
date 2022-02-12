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
	printf("Sizes of data types And pointers to those data types are : \n\n");
	printf("Sizeof(int)                : %ld \t\t\t Sizeof pointer to int (int *)                             : %ld\n\n", sizeof(int), sizeof(int*));
	printf("Sizeof(float)              : %ld \t\t\t Sizeof pointer to float (float *)                         : %ld\n\n", sizeof(float), sizeof(float*));
	printf("Sizeof(double)             : %ld \t\t\t Sizeof pointer to double (double *)                       : %ld\n\n", sizeof(double), sizeof(double*));
	printf("Sizeof(char)               : %ld \t\t\t Sizeof pointer to char (char *)                           : %ld\n\n", sizeof(char), sizeof(char*));
	printf("Sizeof(struct Employee)    : %ld \t\t\t Sizeof pointer to struct Employee (struct Employee *)     : %ld\n\n", sizeof(struct Employee), sizeof(struct Employee*));

	return 0;
}
