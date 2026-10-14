/*
Problem Statement : Copy data from original file to newly created file.
Date	: 11 May 2024
Author	: Shubham Jadhav.

Asked in IIT Madras MS Interview.
*/

// Header Files
#include <stdio.h>

// Global Variable Declarations
FILE* Fp_old;
FILE* Fp_new;

// Function Declaration
FILE* Copy(char* string);
void Uninitialise(void);


// Entry-point Function
int main(void)
{
	// Code 
	FILE* fp = Copy("OriginalFile.txt");

	if (fp != NULL)
		printf("\n----------Data Copied Successfully------------\n\n");

	Uninitialise();

	return 0;
}


// Function Definition
FILE* Copy(char* str)
{
	// Local Variable Declaration
	char ch;

	// Code
	Fp_old = fopen(str,"r");	
	if (Fp_old == NULL)
	{
		printf("\n File Not Exist.\n");
	}

	Fp_new = fopen("Temp.txt", "w");
	if (Fp_new == NULL)
	{
		printf("\n File Creation failed.\n");
	}
	else
	{
		printf("\n File Created Successfully.\n\n");
	}

	while ((ch = fgetc(Fp_old)) != EOF)
	{
		//printf("%c", ch);
		fputc(ch, Fp_new);
	}

	return Fp_new;
}

void Uninitialise(void)
{
	// Code
	if (Fp_new != NULL)
	{
		fclose(Fp_new);
		Fp_new = NULL;
	}

	if (Fp_old != NULL)
	{
		fclose(Fp_old);
		Fp_old = NULL;
		printf("\n File Closed Successfully.\n\n");
	}
}