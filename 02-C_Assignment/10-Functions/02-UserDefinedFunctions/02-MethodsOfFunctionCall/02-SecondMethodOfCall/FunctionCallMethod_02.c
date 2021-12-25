// Function Calling Method 2

#include <stdio.h>

//***** USER DEFINIED FUNCTIONS : METHOD OF CALLING FUNCTION 2 ******
// Calling Only Two FUnctions Directly In main(), Rest Of The Functions Trace Their Call Indirectly To main() *** 

int main(int argc, char* argv[], char* envp[]) {

	// Function prototypes
	void Display_Information(void);
	void Function_Country(void);

	// Code
	Display_Information();
	Function_Country();

	return 0;
}

//*** USER DEFINED FUNCTION'S DEFINITIONS ***

void Display_Information(void) {

	// Function Prototypes
	void Function_My(void);
	void Function_Name(void);
	void Function_Is(void);
	void Function_FirstName(void);
	void Function_MiddleName(void);
	void Function_Surname(void);
	void Function_OfAMC(void);

	// Code

	Function_My();			// Function Calls
	Function_Name();
	Function_Is();
	Function_FirstName();
	Function_MiddleName();
	Function_Surname();
	Function_OfAMC();

}

void Function_My(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("My");
}

void Function_Name(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Name");
}

void Function_Is(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Is");
}

void Function_FirstName(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Shubham");
}

void Function_MiddleName(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Balaso");
}

void Function_Surname(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Jadhav");
}

void Function_OfAMC(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("Of ASTROMEDICOMP");
}

void Function_Country(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("I Live In India\n\n");
}
