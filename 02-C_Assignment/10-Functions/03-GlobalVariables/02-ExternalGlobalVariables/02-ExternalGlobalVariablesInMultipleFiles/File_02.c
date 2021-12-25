// Second File

#include <stdio.h>

//*** GLOBAL SCOPE ***
/* global_count Is A Global Variable Declared In Source Code File ExternalGlobalVariablesInMultipleFiles.c
* 
*  To Access It In Ths File, It must Be First Re-Declared As An External variable In Thw Global Scope Of This File Along With The 'extern' Keyword And The Type Of The Variable.
   Then, It can Be Used As Any Ordinary Global Vriable Throughout This File As Well.
	
	Here, Re-Declaring 'shubh_global_count' As A Local Vriable Using 'extern' Keyword Within change_count_two()
*/


void change_count_two(void) {

	// Code

	extern int shubh_global_count;
	shubh_global_count += 1;
	printf("change_count_Two() : Value of Global Count In File_02 = %d\n", shubh_global_count);
}
