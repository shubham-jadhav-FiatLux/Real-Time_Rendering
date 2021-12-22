// Program For 'for' Loop Where User Input Dependent Iteration Performed

#include <stdio.h>

int main(void) {

	// Variable Declarations
	int shubh_start, shubh_num, shubh_i;

	// Code
	printf("\n\n");

	printf("Enter AN Integer Value From Which Iteration Must Begins : ");
	scanf("%d", &shubh_start);

	printf("How Many Digits Do You Want To Print From %d Onwards ? : ", shubh_start);
	scanf("%d", &shubh_num);

	printf("Printing Digits %d To %d : \n\n", shubh_start, (shubh_start + shubh_num));

	for (shubh_i = shubh_start; shubh_i <= (shubh_start + shubh_num); shubh_i++) {

		printf("\t%d\n", shubh_i);
	}

	printf("\n\n");

	return 0;
}
