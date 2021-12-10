// Function calling without declaration

#include <stdio.h>

//double foo(double);

int main(void) {

	double da, db;
	//input da
	db = foo(da);
	return 0;
}

double foo(double a) {			//Compiler error

	return a;
}

// Bydefault return type of function is 'int'.
// so, when we call function without declaration then compiler consider as int like " int foo(double) " in our case.
// Due to this compiler gives type mismatch error.