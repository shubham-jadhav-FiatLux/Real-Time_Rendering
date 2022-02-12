#include <stdio.h>

#define NAME_LENGTH 100
#define	MARITAL_STATUS 10

struct Employee {

	char shubh_name[NAME_LENGTH];
	int shubh_age;
	float shubh_salary;
	char shubh_sex;
	char shubh_maritalStatus[MARITAL_STATUS];
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct Employee EmployeeRecord[5];

	char employee_shubham[] = "Shubham";
	char employee_prasad[] = "Prasad";
	char employee_umesh[] = "Umesh";
	char employee_rishi[] = "Rishi";
	char employee_girish[] = "Girish";

	int shubh_i;

	// Code
	// Hard coded initialization of array of struct Employee

	// ******* Employee 1 ********
	strcpy(EmployeeRecord[0].shubh_name, employee_shubham);
	EmployeeRecord[0].shubh_age = 22;
	EmployeeRecord[0].shubh_sex = 'M';
	EmployeeRecord[0].shubh_salary = 50000000.0f;
	strcpy(EmployeeRecord[0].shubh_maritalStatus, "Unmarried");
	
	// ******* Employee 2 ********
	strcpy(EmployeeRecord[1].shubh_name, employee_prasad);
	EmployeeRecord[1].shubh_age = 21;
	EmployeeRecord[1].shubh_sex = 'M';
	EmployeeRecord[1].shubh_salary = 600000.0f;
	strcpy(EmployeeRecord[1].shubh_maritalStatus, "Unmarried");

	// ******* Employee 3 ********
	strcpy(EmployeeRecord[2].shubh_name, employee_umesh);
	EmployeeRecord[2].shubh_age = 25;
	EmployeeRecord[2].shubh_sex = 'M';
	EmployeeRecord[2].shubh_salary = 58000.0f;
	strcpy(EmployeeRecord[2].shubh_maritalStatus, "Unmarried");

	// ******* Employee 4 ********
	strcpy(EmployeeRecord[3].shubh_name, employee_rishi);
	EmployeeRecord[3].shubh_age = 29;
	EmployeeRecord[3].shubh_sex = 'M';
	EmployeeRecord[3].shubh_salary = 900000.0f;
	strcpy(EmployeeRecord[3].shubh_maritalStatus, "married");

	// ******* Employee 5 ********
	strcpy(EmployeeRecord[4].shubh_name, employee_girish);
	EmployeeRecord[4].shubh_age = 18;
	EmployeeRecord[4].shubh_sex = 'M';
	EmployeeRecord[4].shubh_salary = 10000.0f;
	strcpy(EmployeeRecord[4].shubh_maritalStatus, "married");


	printf("\n\n");
	printf("**** Displaying Employee Records ****\n\n");
	for (shubh_i = 0; shubh_i < 5; shubh_i++) {
	
		printf("****** Empolyee Number %d ******\n\n", (shubh_i + 1));
		printf("Name   : %s\n", EmployeeRecord[shubh_i].shubh_name);
		printf("Age   : %d years\n", EmployeeRecord[shubh_i].shubh_age);
		
		if (EmployeeRecord[shubh_i].shubh_sex == 'M' || EmployeeRecord[shubh_i].shubh_sex == 'm')
			printf("Sex  : Male\n");
		else
			printf("Sex  : Female\n");

		printf("Salary   : Rs %f\n", EmployeeRecord[shubh_i].shubh_salary);
		printf("Marital Status   : %s\n", EmployeeRecord[shubh_i].shubh_maritalStatus);
	
		printf("\n\n");
	}
	return 0;
}
