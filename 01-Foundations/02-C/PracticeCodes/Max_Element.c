/*
Problem Statement : To find out maximum element in array using recursive method.
Date   : 11 May 2024
Author : Shubham Jadhav

Asked in IIT Madras MS Research Interview.
*/

#include <stdio.h>

int max = 0;

int Max(int N, int* arr);

int main(void)
{
	int arr[] = {-2, -5, -8, -10 , -1, -6, -4, -3};

	max = arr[0];
	Max(sizeof(arr)/sizeof(int), arr);

	printf("Max element is %d\n", max);

	return 0;
}

int Max(int N, int* arr)
{
	//static int max = -100;

	if (N == 0)
		return 0;

	if (arr[N - 1] >= max)
		max = arr[N - 1];
	
	Max(N-1, arr);

	return max;
}