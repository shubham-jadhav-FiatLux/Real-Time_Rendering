#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char shubh_ch, shubh_ch_i;
	unsigned int shubh_ascii_ch = 0;

	// Code
	printf("\n\n");
	printf("Enter the first character of first name : ");
	shubh_ch = getch();

	shubh_ch = toupper(shubh_ch);

	for (shubh_ch_i = 'A'; shubh_ch_i <= 'Z'; shubh_ch_i++) {
	
		if (shubh_ch == shubh_ch_i) {
		
			shubh_ascii_ch = (unsigned int)shubh_ch;
			goto result_output;
		}
	}

	printf("\n\n");
	printf("Goto statement not executed, so printing \"Hello World !!\"\n");

result_output:
	printf("\n\n");

	if (shubh_ascii_ch == 0) {
	
		printf("You must have a strange name !Could not find the character '%c' in the entire English alphabet !\n", shubh_ch);
	}
	else {
	
		printf("Character '%c' found. It has ASCII value %u.\n", shubh_ch, shubh_ascii_ch);
	}
	printf("\n\n");

	return 0;
}
