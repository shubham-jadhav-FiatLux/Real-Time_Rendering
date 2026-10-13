// Command Line Arguments Application 

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	int shubh_i;

	// Code
	printf("\n\n");

	if (argc != 4) {		// Program Name + First Name + Middle Name + Surname = 4 Command Line Arguments Are Required
	
		printf("Invalid Usage !! Exitting Now...\n\n");
		printf("Usage : ComandLineArgumentsApplication <first name> <middle name> <surname>\n\n");
		exit(0);
	}

	//***This Program Prints Your Full Name As Entered In The Command Line Arguents***

	printf("Your Full Name Is : ");

	// Loop Starts From i=1 Because, i=0 Will Result In 'argv[i]' = 'argv[0]' Which Is The Name Of The Program Itself i.e. : 'ComandLineArgumentsApplication.exe'

	for (shubh_i = 1; shubh_i < argc; shubh_i++) {
	
		printf("%s ", argv[shubh_i]);
	}

	printf("\n\n");

	return 0;
}
