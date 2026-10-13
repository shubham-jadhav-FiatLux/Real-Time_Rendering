// Function Calling Method 3

#include <stdio.h>

//***** USER DEFINIED FUNCTIONS : METHOD OF CALLING FUNCTION 3 ******
// Calling Only One FUnctions Directly In main(), Rest Of The Functions Trace Their Call Indirectly To main() *** 

int main(int argc, char* argv[], char* envp[]) {

	// Function prototypes
	void Function_Country(void);

	// Code
	Function_Country();

	return 0;
}

//*** USER DEFINED FUNCTION'S DEFINITIONS ***

void Function_Country(void) {		// Function Definition

	// Function Prototypes
	void Function_OfAMC(void);

	// Code
	Function_OfAMC();

	printf("\n\n");

	printf("I Live In India\n\n");
}


void Function_OfAMC(void) {		// Function Definition

	// Function Prototypes
	void Function_Surname(void);


	// Code
	Function_Surname();

	printf("\n\n");

	printf("Of ASTROMEDICOMP");
}


void Function_Surname(void) {		// Function Definition

	// Function Prototypes
	void Function_MiddleName(void);

	// Code
	Function_MiddleName();

	printf("\n\n");

	printf("Jadhav");
}


void Function_MiddleName(void) {		// Function Definition

	// Function Prototypes
	void Function_FirstName(void);

	// Code
	Function_FirstName();

	printf("\n\n");

	printf("Balaso");
}

void Function_FirstName(void) {		// Function Definition

	// Function Prototypes
	void Function_Is(void);

	// Code
	Function_Is();

	printf("\n\n");

	printf("Shubham");
}


void Function_Is(void) {		// Function Definition

	// Function Prototypes
	void Function_Name(void);

	// Code
	Function_Name();

	printf("\n\n");

	printf("Is");
}


void Function_Name(void) {		// Function Definition

	// Function Prototypes
	void Function_My(void);

	// Code
	Function_My();			// Function Calls

	printf("\n\n");

	printf("Name");
}


void Function_My(void) {		// Function Definition

	// Code
	printf("\n\n");

	printf("My");
}
