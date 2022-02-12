#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
	int* shubh_ptr_iArray = NULL;

	// Code
	printf("\n\n");
	printf("*** Using array name as pointer i.e. : Value of xth element of array : *(shubh_iArray + x) and address of xth element of array : (shubh_iArray + x) ***\n\n");

	printf("Integer array elements and their addresses : \n\n");
	printf("*(shubh_iArray + 0) = %d \t\t at address (shubh_iArray + 0) : %p\n",*( shubh_iArray + 0), (shubh_iArray + 0));
	printf("*(shubh_iArray + 1) = %d \t\t at address (shubh_iArray + 1) : %p\n", *(shubh_iArray + 1), (shubh_iArray + 1));
	printf("*(shubh_iArray + 2) = %d \t\t at address (shubh_iArray + 2) : %p\n", *(shubh_iArray + 2), (shubh_iArray + 2));
	printf("*(shubh_iArray + 3) = %d \t\t at address (shubh_iArray + 3) : %p\n", *(shubh_iArray + 3), (shubh_iArray + 3));
	printf("*(shubh_iArray + 4) = %d \t\t at address (shubh_iArray + 4) : %p\n", *(shubh_iArray + 4), (shubh_iArray + 4));
	printf("*(shubh_iArray + 5) = %d \t\t at address (shubh_iArray + 5) : %p\n", *(shubh_iArray + 5), (shubh_iArray + 5));
	printf("*(shubh_iArray + 6) = %d \t\t at address (shubh_iArray + 6) : %p\n", *(shubh_iArray + 6), (shubh_iArray + 6));
	printf("*(shubh_iArray + 7) = %d \t\t at address (shubh_iArray + 7) : %p\n", *(shubh_iArray + 7), (shubh_iArray + 7));
	printf("*(shubh_iArray + 8) = %d \t\t at address (shubh_iArray + 8) : %p\n", *(shubh_iArray + 8), (shubh_iArray + 8));
	printf("*(shubh_iArray + 9) = %d \t\t at address (shubh_iArray + 9) : %p\n", *(shubh_iArray + 9), (shubh_iArray + 9));

	shubh_ptr_iArray = shubh_iArray;

	printf("\n\n");
	printf("*** Using pointer as array name i.e. : Value of xth element of array : shubh_ptr_iArray[x] and address of xth element of array : &shubh_ptr_iArray[x] ***\n\n");

	printf("Integer array elements and their addresses : \n\n");
	printf("shubh_ptr_iArray[0] = %d \t\t at address &shubh_ptr_iArray[0] : %p\n", shubh_ptr_iArray[0], &shubh_ptr_iArray[0]); 
	printf("shubh_ptr_iArray[1] = %d \t\t at address &shubh_ptr_iArray[1] : %p\n", shubh_ptr_iArray[1], &shubh_ptr_iArray[1]); 
	printf("shubh_ptr_iArray[2] = %d \t\t at address &shubh_ptr_iArray[2] : %p\n", shubh_ptr_iArray[2], &shubh_ptr_iArray[2]);
	printf("shubh_ptr_iArray[3] = %d \t\t at address &shubh_ptr_iArray[3] : %p\n", shubh_ptr_iArray[3], &shubh_ptr_iArray[3]);
	printf("shubh_ptr_iArray[4] = %d \t\t at address &shubh_ptr_iArray[4] : %p\n", shubh_ptr_iArray[4], &shubh_ptr_iArray[4]);
	printf("shubh_ptr_iArray[5] = %d \t\t at address &shubh_ptr_iArray[5] : %p\n", shubh_ptr_iArray[5], &shubh_ptr_iArray[5]);
	printf("shubh_ptr_iArray[6] = %d \t\t at address &shubh_ptr_iArray[6] : %p\n", shubh_ptr_iArray[6], &shubh_ptr_iArray[6]);
	printf("shubh_ptr_iArray[7] = %d \t\t at address &shubh_ptr_iArray[7] : %p\n", shubh_ptr_iArray[7], &shubh_ptr_iArray[7]);
	printf("shubh_ptr_iArray[8] = %d \t\t at address &shubh_ptr_iArray[8] : %p\n", shubh_ptr_iArray[8], &shubh_ptr_iArray[8]);
	printf("shubh_ptr_iArray[9] = %d \t\t at address &shubh_ptr_iArray[9] : %p\n", shubh_ptr_iArray[9], &shubh_ptr_iArray[9]);
	
	return 0;
}
