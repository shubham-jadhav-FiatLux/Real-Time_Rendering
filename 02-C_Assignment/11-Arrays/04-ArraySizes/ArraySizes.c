#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_iArrayOne[5];
	int shubh_iArrayTwo[5][3];
	int shubh_iArrayThree[100][100][5];

	int shubh_NumRows2D;
	int shubh_NumColumns2D;

	int shubh_NumRows3D;
	int shubh_NumColumns3D;
	int shubh_Depth3D;

	// Code
	printf("\n\n");
	printf("Size of 1-D integer array shubh_iArrayOne = %lu\n", sizeof(shubh_iArrayOne));
	printf("Number of elements in 1-D integer array shubh_iArrayOne = %lu\n", (sizeof(shubh_iArrayOne)/ sizeof(int)));

	printf("\n\n");
	printf("Size of 2-D integer array shubh_iArrayTwo = %lu\n", sizeof(shubh_iArrayTwo));
	printf("Number of rows in 2-D integer array shubh_iArrayTwo = %lu\n", (sizeof(shubh_iArrayTwo) / sizeof(shubh_iArrayTwo[0])));
	shubh_NumRows2D = (sizeof(shubh_iArrayTwo) / sizeof(shubh_iArrayTwo[0]));

	printf("Number of elements (columns) in each row of 2-D integer array shubh_iArrayTwo = %lu\n", (sizeof(shubh_iArrayTwo[0]) / sizeof(shubh_iArrayTwo[0][0])));
	shubh_NumColumns2D = (sizeof(shubh_iArrayTwo[0]) / sizeof(shubh_iArrayTwo[0][0]));

	printf("Number of elements in total 2-D integer array shubh_iArrayTwo = %d\n", (shubh_NumRows2D * shubh_NumColumns2D));

	printf("\n\n");
	


	printf("\n\n");
	printf("Size of 3-D integer array shubh_iArrayThree = %lu\n", sizeof(shubh_iArrayThree));
	printf("Number of rows in 3-D integer array shubh_iArrayThree = %lu\n", (sizeof(shubh_iArrayThree) / sizeof(shubh_iArrayThree[0])));
	shubh_NumRows3D = (sizeof(shubh_iArrayThree) / sizeof(shubh_iArrayThree[0]));

	printf("Number of elements (columns) in each row of 3-D integer array shubh_iArrayThree = %lu\n", (sizeof(shubh_iArrayThree[0]) / sizeof(shubh_iArrayThree[0][0])));
	shubh_NumColumns3D = (sizeof(shubh_iArrayThree[0]) / sizeof(shubh_iArrayThree[0][0]));

	printf("Number of elements (depth) in one column in one row in 3-D integer array shubh_iArrayThree = %lu\n", (sizeof(shubh_iArrayThree[0][0]) / sizeof(shubh_iArrayThree[0][0][0])));
	shubh_Depth3D = (sizeof(shubh_iArrayThree[0][0]) / sizeof(shubh_iArrayThree[0][0][0]));

	printf("Number of elements in total 3-D integer array shubh_iArrayThree = %d\n", (shubh_NumRows3D * shubh_NumColumns3D * shubh_Depth3D));

	printf("\n\n");
	
	return 0;
}
