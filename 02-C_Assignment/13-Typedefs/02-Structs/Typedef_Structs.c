#include <stdio.h>

#define MAX_NAME_LENGTH  100

struct Employee {

	char shubh_name[MAX_NAME_LENGTH];
	unsigned int shubh_age;
	char shubh_gender;
	double shubh_salary;
};

struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {

	// Typedefs
	typedef struct Employee MY_EMPLOYEE_TYPE;
	typedef struct MyData MY_DATA_TYPE;

	// Variable Declarations
	struct Employee emp = {"Funny", 25, 'M', 10000.00};
	MY_EMPLOYEE_TYPE emp_typedef = { "Bunny", 23, 'F', 20000.00 };

	struct MyData md = {30, 11.45f, 26.122017, 'X'};
	MY_DATA_TYPE md_typedef;

	// Code
	md_typedef.shubh_i = 9;
	md_typedef.shubh_f = 1.5f;
	md_typedef.shubh_d = 8.041997;
	md_typedef.shubh_c = 'P';

	printf("\n\n");
	printf("struct Employee : \n\n");
	printf("emp.name   = %s\n", emp.shubh_name);
	printf("emp.age    = %d\n", emp.shubh_age);
	printf("emp.gender = %c\n", emp.shubh_gender);
	printf("emp.salary = %lf\n", emp.shubh_salary);


	printf("\n\n");
	printf("MY_EMPLOYEE_TYPE : \n\n");
	printf("emp_typedef.name   = %s\n", emp_typedef.shubh_name);
	printf("emp_typedef.age    = %d\n", emp_typedef.shubh_age);
	printf("emp_typedef.gender = %c\n", emp_typedef.shubh_gender);
	printf("emp_typedef.salary = %lf\n", emp_typedef.shubh_salary);


	printf("\n\n");
	printf("struct MyData : \n\n");
	printf("md.i   = %d\n", md.shubh_i);
	printf("md.f    = %f\n", md.shubh_f);
	printf("md.d = %lf\n", md.shubh_d);
	printf("md.c = %c\n", md.shubh_c);

	printf("\n\n");
	printf("MY_DATA_TYPE : \n\n");
	printf("md_typedef.i  = %d\n", md_typedef.shubh_i);
	printf("md_typedef.f  = %f\n", md_typedef.shubh_f);
	printf("md_typedef.d  = %lf\n", md_typedef.shubh_d);
	printf("md_typedef.c  = %c\n", md_typedef.shubh_c);

	printf("\n\n");

	return 0;
}
