// Program to parse the given identifier and checks whether it is correct or not

#include <stdio.h>

// Function Declarations
void start(char *);
void go(char *);
void error(char *);

// Global Variable Declarations
char* temp = NULL;

int main(void) {

	// Variable Declaration
	char p[32];

	// Code
	printf("\n\n*** Recursive Descent Parser For Identifiers ***\n\n");
	printf("\n\nEnter Your Identifier Name :	");
	//scanf("%s", p);
	gets(p);
	temp = p;
	start(p);

	return 0;
}

void start(char *p) {

	// Code
	if(strlen(p)<= 32 )
	
		if ( match(p[0]) )
			go(++p);
	
	error(temp);			// If starting symbol contain special symbol other than '_' or integer  or 
							// length exceeds 32 then error.
}

void go(char* p) {

	// Code
	for (int i = 0; i < strlen(p); i++) {

		if ( p[i] >= 48 && p[i] <= 57 || match(p[i]) )
			continue;

		error(temp);			// If identifier contain special symbol or space ' ' then error.
	}

	printf("\nIdentifier Name \'%s\' Is Valid\n\n", temp);
	exit(0);
}

int match(char ch) {

	//Code
	if (ch == '_' || ch >= 65 && ch <= 90 || ch >= 97 && ch <= 122)
		return 1;
	else 
		return 0;
}

void error(char *p) {

	// Code
	printf("\nSyntax Error : Identifier Name \'%s\' Is Not Valid !!!\n\n", p);
	exit(0);
}
