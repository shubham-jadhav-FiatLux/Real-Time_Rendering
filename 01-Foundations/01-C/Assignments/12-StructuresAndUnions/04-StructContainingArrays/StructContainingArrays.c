#include <stdio.h>

#define INT_ARRAY_SIZE 10
#define FLOAT_ARRAY_SIZE 5
#define CHAR_ARRAY_SIZE 26

#define NUM_STRINGS 10
#define MAX_CHARACTERS_PER_STRING 20

#define ALPHABET_BEGINNING 65

struct MyDataOne {

	int shubh_iArray[INT_ARRAY_SIZE];
	float shubh_fArray[FLOAT_ARRAY_SIZE];
};

struct MyDataTwo {

	char shubh_cArray[CHAR_ARRAY_SIZE];
	char shubh_strArray[NUM_STRINGS][MAX_CHARACTERS_PER_STRING];
};

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	struct MyDataOne data_one;
	struct MyDataTwo data_two;
	int shubh_i;

	// Code
	data_one.shubh_fArray[0] = 0.1f;
	data_one.shubh_fArray[1] = 1.2f;
	data_one.shubh_fArray[2] = 2.3f;
	data_one.shubh_fArray[3] = 3.4f;
	data_one.shubh_fArray[4] = 4.5f;

	printf("\n\n");
	printf("Enter %d Integers : \n\n", INT_ARRAY_SIZE);
	for (shubh_i = 0; shubh_i < INT_ARRAY_SIZE; shubh_i++)
		scanf("%d", &data_one.shubh_iArray[shubh_i]);

	for (shubh_i = 0; shubh_i < CHAR_ARRAY_SIZE; shubh_i++)
		data_two.shubh_cArray[shubh_i] = (char)(shubh_i + ALPHABET_BEGINNING);

	strcpy(data_two.shubh_strArray[0], "Welcome !!");
	strcpy(data_two.shubh_strArray[1], "This");
	strcpy(data_two.shubh_strArray[2], "Is");
	strcpy(data_two.shubh_strArray[3], "ASTROMEDICOMP'S");
	strcpy(data_two.shubh_strArray[4], "Real");
	strcpy(data_two.shubh_strArray[5], "Time");
	strcpy(data_two.shubh_strArray[6], "Rendering");
	strcpy(data_two.shubh_strArray[7], "Batch");
	strcpy(data_two.shubh_strArray[8], "Of");
	strcpy(data_two.shubh_strArray[9], "2020-2021 !!");

	printf("\n\n");
	printf("Members of 'struct DataOne' alongwith their assigned values are : \n\n");

	printf("\n\n");
	printf("Integer Array (data_one.shubh_iArray[]) : \n\n", INT_ARRAY_SIZE);
	for (shubh_i = 0; shubh_i < INT_ARRAY_SIZE; shubh_i++)
		printf("data_one.shubh_iArray[%d] = %d\n", shubh_i, data_one.shubh_iArray[shubh_i]);

	printf("\n\n");
	printf("Floating-Point Array (data_one.shubh_fArray[]) : \n\n", INT_ARRAY_SIZE);
	for (shubh_i = 0; shubh_i < FLOAT_ARRAY_SIZE; shubh_i++)
		printf("data_one.shubh_fArray[%d] = %f\n", shubh_i, data_one.shubh_fArray[shubh_i]);

	

	printf("\n\n");
	printf("Members of 'struct DataTwo' alongwith their assigned values are : \n\n");

	printf("\n\n");
	printf("Character Array (data_one.shubh_cArray[]) : \n\n", INT_ARRAY_SIZE);
	for (shubh_i = 0; shubh_i < CHAR_ARRAY_SIZE; shubh_i++)
		printf("data_two.shubh_cArray[%d] = %d\n", shubh_i, data_two.shubh_cArray[shubh_i]);

	printf("\n\n");
	printf("String Array (data_two.shubh_strArray[]) : \n\n", INT_ARRAY_SIZE);
	for (shubh_i = 0; shubh_i < NUM_STRINGS; shubh_i++)
		printf("%s ", data_two.shubh_strArray[shubh_i]);

	printf("\n\n");

	return 0;
}
