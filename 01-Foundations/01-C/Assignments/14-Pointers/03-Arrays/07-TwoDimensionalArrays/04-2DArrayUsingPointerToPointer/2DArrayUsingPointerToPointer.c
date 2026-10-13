#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int** shubh_ptr_iArray = NULL;
	int shubh_i, shubh_j;
	int num_rows, num_columns;

	// Code
	printf("\n\n");
	printf("Enter number of rows : ");
	scanf("%d", &num_rows);

	printf("\n\n");
	printf("Enter number of columns : ");
	scanf("%d", &num_columns);

	printf("\n\n");
	printf("************ Memory allocation to 2D integer array **************\n\n");
	shubh_ptr_iArray = (int**)malloc(num_rows * sizeof(int*));
	if (shubh_ptr_iArray == NULL) {

		printf("Failed to allocate memory to row %d of 2D integer array!! .Exiting ...\n\n", num_rows);
		exit(0);
	}
	else
		printf("Memory allocation to row %d of 2D integer array succeeded !!\n\n", num_rows);


	for (shubh_i = 0; shubh_i < num_rows; shubh_i++) {

		shubh_ptr_iArray[shubh_i] = (int*)malloc(num_columns * sizeof(int));
		if (shubh_ptr_iArray[shubh_i] == NULL) {

			printf("Failed to allocate memory to row %d of 2D integer array!! .Exiting ...\n\n", shubh_i);
			exit(0);
		}
		else
			printf("Memory allocation to row %d of 2D integer array succeeded !!\n\n", shubh_i);
	}

	for (shubh_i = 0; shubh_i < num_rows; shubh_i++) {

		for (shubh_j = 0; shubh_j < num_columns; shubh_j++) {

			shubh_ptr_iArray[shubh_i][shubh_j] = (shubh_i + 1) * (shubh_j + 1);
		}
	}

	for (shubh_i = 0; shubh_i < num_rows; shubh_i++) {

		printf("Base address of rows %d : shubh_ptr_iArray[%d] = %p \t at address : %p\n", shubh_i, shubh_i, shubh_ptr_iArray[shubh_i], &shubh_ptr_iArray[shubh_i]);
	}

	printf("\n\n");
	for (shubh_i = 0; shubh_i < num_rows; shubh_i++) {

		for (shubh_j = 0; shubh_j < num_columns; shubh_j++) {

			printf("shubh_ptr_iArray[%d][%d] = %d \t at address : %p\n", shubh_i, shubh_j, shubh_ptr_iArray[shubh_i][shubh_j], &shubh_ptr_iArray[shubh_i][shubh_j]);
		}
		printf("\n\n");
	}

	printf("\n\n");

	// Freeing memory of each row
	for (shubh_i = (num_rows - 1); shubh_i >= 0; shubh_i--) {

		if (shubh_ptr_iArray[shubh_i]) {

			free(shubh_ptr_iArray[shubh_i]);
			shubh_ptr_iArray[shubh_i] = NULL;
			printf("Memory allocated to row %d of 2D integer array has been successfully freed !!\n\n", shubh_i);
		}
	}


	if (shubh_ptr_iArray) {

		free(shubh_ptr_iArray);
		shubh_ptr_iArray = NULL;
		printf("Memory allocated to shubh_ptr_iArray has been successfully freed !!\n\n");
	}

	return 0;
}

