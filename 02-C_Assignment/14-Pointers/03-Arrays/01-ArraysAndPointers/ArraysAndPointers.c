#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArray[] = { 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 };
	float shubh_fArray[] = {9.8f, 8.7f, 7.6f, 6.5f, 5.4f};
	double shubh_dArray[] = {1.22222, 2.33333, 3.44444};
	char shubh_cArray[] = {'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0'};

	// Code
	printf("\n\n");
	printf("Integer array elments and the addresses they occupy are as follows : \n\n");
	printf("shubh_iArray[0] = %d \t at address : %p\n", *(shubh_iArray + 0), (shubh_iArray + 0));
	printf("shubh_iArray[1] = %d \t at address : %p\n", *(shubh_iArray + 1), (shubh_iArray + 1)); 
	printf("shubh_iArray[2] = %d \t at address : %p\n", *(shubh_iArray + 2), (shubh_iArray + 2)); 
	printf("shubh_iArray[3] = %d \t at address : %p\n", *(shubh_iArray + 3), (shubh_iArray + 3)); 
	printf("shubh_iArray[4] = %d \t at address : %p\n", *(shubh_iArray + 4), (shubh_iArray + 4)); 
	printf("shubh_iArray[5] = %d \t at address : %p\n", *(shubh_iArray + 5), (shubh_iArray + 5)); 
	printf("shubh_iArray[6] = %d \t at address : %p\n", *(shubh_iArray + 6), (shubh_iArray + 6)); 
	printf("shubh_iArray[7] = %d \t at address : %p\n", *(shubh_iArray + 7), (shubh_iArray + 7)); 
	printf("shubh_iArray[8] = %d \t at address : %p\n", *(shubh_iArray + 8), (shubh_iArray + 8)); 
	printf("shubh_iArray[9] = %d \t at address : %p\n", *(shubh_iArray + 9), (shubh_iArray + 9)); 

	printf("\n\n");
	printf("Float array elments and the addresses they occupy are as follows : \n\n");
	printf("shubh_fArray[0] = %f \t at address : %p\n", *(shubh_fArray + 0), (shubh_fArray + 0));
	printf("shubh_fArray[1] = %f \t at address : %p\n", *(shubh_fArray + 1), (shubh_fArray + 1)); 
	printf("shubh_fArray[2] = %f \t at address : %p\n", *(shubh_fArray + 2), (shubh_fArray + 2)); 
	printf("shubh_fArray[3] = %f \t at address : %p\n", *(shubh_fArray + 3), (shubh_fArray + 3)); 
	printf("shubh_fArray[4] = %f \t at address : %p\n", *(shubh_fArray + 4), (shubh_fArray + 4)); 

	printf("\n\n");
	printf("Double array elments and the addresses they occupy are as follows : \n\n");
	printf("shubh_dArray[0] = %lf \t at address : %p\n", *(shubh_dArray + 0), (shubh_dArray + 0));
	printf("shubh_dArray[1] = %lf \t at address : %p\n", *(shubh_dArray + 1), (shubh_dArray + 1));
	printf("shubh_dArray[2] = %lf \t at address : %p\n", *(shubh_dArray + 2), (shubh_dArray + 2));

	printf("\n\n");
	printf("Character array elments and the addresses they occupy are as follows : \n\n");
	printf("shubh_cArray[0] = %c \t at address : %p\n", *(shubh_cArray + 0), (shubh_cArray + 0));
	printf("shubh_cArray[1] = %c \t at address : %p\n", *(shubh_cArray + 1), (shubh_cArray + 1)); 
	printf("shubh_cArray[2] = %c \t at address : %p\n", *(shubh_cArray + 2), (shubh_cArray + 2)); 
	printf("shubh_cArray[3] = %c \t at address : %p\n", *(shubh_cArray + 3), (shubh_cArray + 3)); 
	printf("shubh_cArray[4] = %c \t at address : %p\n", *(shubh_cArray + 4), (shubh_cArray + 4)); 
	printf("shubh_cArray[5] = %c \t at address : %p\n", *(shubh_cArray + 5), (shubh_cArray + 5)); 
	printf("shubh_cArray[6] = %c \t at address : %p\n", *(shubh_cArray + 6), (shubh_cArray + 6)); 
	printf("shubh_cArray[7] = %c \t at address : %p\n", *(shubh_cArray + 7), (shubh_cArray + 7)); 
	printf("shubh_cArray[8] = %c \t at address : %p\n", *(shubh_cArray + 8), (shubh_cArray + 8)); 
	printf("shubh_cArray[9] = %c \t at address : %p\n", *(shubh_cArray + 9), (shubh_cArray + 9));
	printf("shubh_cArray[10] = %c \t at address : %p\n", *(shubh_cArray + 10), (shubh_cArray + 10)); 
	printf("shubh_cArray[11] = %c \t at address : %p\n", *(shubh_cArray + 11), (shubh_cArray + 11)); 
	printf("shubh_iArray[12] = %c \t at address : %p\n", *(shubh_iArray + 12), (shubh_iArray + 12));
	printf("shubh_iArray[13] = %c \t at address : %p\n", *(shubh_iArray + 13), (shubh_iArray + 13));

	return 0;
}
