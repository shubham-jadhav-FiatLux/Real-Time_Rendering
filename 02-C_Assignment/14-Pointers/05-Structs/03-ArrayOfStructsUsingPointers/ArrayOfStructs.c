#include <stdio.h>
#include <ctype.h>

#define NAME_LENGTH 100
#define	MARITAL_STATUS 10

struct Employee {

	char shubh_name[NAME_LENGTH];
	int shubh_age;
	float shubh_salary;
	char shubh_sex;
	char shubh_maritalStatus;
};

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	void MyGetString(char[], int);

	// Variable Declarations
	struct Employee *pEmployeeRecord = NULL;
	int shubh_i, num_employees;

	// Code
	printf("\n\n");
	printf("Enter number of employees whose details you want to record : ");
	scanf("%d", &num_employees);

	printf("\n\n");
	pEmployeeRecord = (struct Employee*)malloc(sizeof(struct Employee));
	if (pEmployeeRecord == NULL) {

		printf("Failed to allocate memory for %d employees!!. Exiting ...\n\n", num_employees);
		exit(0);
	}
	else
		printf("Successfully allocated memory for %d employees!!.\n\n", num_employees);


	for (shubh_i = 0; shubh_i < num_employees; shubh_i++) {

		printf("\n\n\n");
		printf("******* Data entry for employee number %d ******\n", (shubh_i + 1));

		printf("\n\n");
		printf("Enter Employee Name : ");
		MyGetString(pEmployeeRecord[shubh_i].shubh_name, NAME_LENGTH);

		printf("\n\n");
		printf("Enter Employee's Age (in years) : ");
		scanf("%d", &pEmployeeRecord[shubh_i].shubh_age);

		printf("\n\n");
		printf("Enter Employee's Sex (M/m for Male , F/f for Female) : ");
		pEmployeeRecord[shubh_i].shubh_sex = getch();
		printf("%c", pEmployeeRecord[shubh_i].shubh_sex);
		pEmployeeRecord[shubh_i].shubh_sex = toupper(pEmployeeRecord[shubh_i].shubh_sex);

		printf("\n\n");
		printf("Enter Employee's Salary (In Indian Rupees) : ");
		scanf("%f", &pEmployeeRecord[shubh_i].shubh_salary);

		printf("\n\n");
		printf("Is the employee married ? (Y/y For Yes, N/n For No) : ");
		pEmployeeRecord[shubh_i].shubh_maritalStatus = getch();
		printf("%c", pEmployeeRecord[shubh_i].shubh_maritalStatus);
		pEmployeeRecord[shubh_i].shubh_maritalStatus = toupper(pEmployeeRecord[shubh_i].shubh_maritalStatus);
	}

	printf("\n\n\n");
	printf("**** Displaying Employee Records ****\n\n");
	for (shubh_i = 0; shubh_i < num_employees; shubh_i++) {

		printf("****** Empolyee Number %d ******\n\n", (shubh_i + 1));
		printf("Name   : %s\n", pEmployeeRecord[shubh_i].shubh_name);
		printf("Age   : %d years\n", pEmployeeRecord[shubh_i].shubh_age);

		if (pEmployeeRecord[shubh_i].shubh_sex == 'M' || pEmployeeRecord[shubh_i].shubh_sex == 'm')
			printf("Sex  : Male\n");
		else
			printf("Sex  : Female\n");

		printf("Salary   : Rs %f\n", pEmployeeRecord[shubh_i].shubh_salary);

		if (pEmployeeRecord[shubh_i].shubh_maritalStatus == 'Y' || pEmployeeRecord[shubh_i].shubh_maritalStatus == 'y')
			printf("Marital Status   : Married\n");
		else if (pEmployeeRecord[shubh_i].shubh_maritalStatus == 'N' || pEmployeeRecord[shubh_i].shubh_maritalStatus == 'n')
			printf("Marital Status   : Unmarried\n");
		else
			printf("Marital Status   : Invalid Data Entered\n");

		printf("\n\n");
	}
	
	if (pEmployeeRecord) {

		free(pEmployeeRecord);
		pEmployeeRecord = NULL;
		printf("Memory allocated to %d employees has been successfully freed !!\n\n", num_employees);
	}

	return 0;
}

void MyGetString(char str[], int str_size) {

	// Variable Declarations
	int shubh_i;
	char shubh_ch = '\0';

	// Code
	shubh_i = 0;
	do {

		shubh_ch = getch();
		str[shubh_i] = shubh_ch;
		printf("%c", str[shubh_i]);
		shubh_i++;
	} while ((shubh_ch != '\r') && (shubh_i < str_size));

	if (shubh_i == str_size)
		str[shubh_i - 1] = '\0';
	else
		str[shubh_i] = '\0';
}
