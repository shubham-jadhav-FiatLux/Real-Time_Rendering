// Second File

#include <stdio.h>

//*** GLOBAL SCOPE ***
/* global_count Is A Global Variable Declared In Source Code File ExternalGlobalVariablesInMultipleFiles.c
*  To Access It In Ths File, It must Be First Re-Declared As An External variable In Thw Global Scope Of This File Along With The 'extern' Keyword And The Type Of The Variable.
   Then, It can Be Used As Any Ordinary Global Vriable Throughout This File As Well.	
*/

extern int shubh_global_count;

void change_count_one(void) {

	// Code
	shubh_global_count += 1;
	printf("change_count_one() : Value of Global Count In File_01 = %d\n", shubh_global_count);
}
