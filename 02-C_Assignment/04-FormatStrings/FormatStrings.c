// program for format strings

#include <stdio.h>

int main(void) {

	// Code
	printf("\n\n");
	printf("************************************************************************************************");
	printf("\n\n");

	printf("HelloWorld !!!\n\n");

	int a = 13;
	printf("Integer Decimal Value of 'a' = %d\n", a);
	printf("Integer Octal value of 'a' = %o\n", a);
	printf("Integer Hexadecimal value of 'a' (Hexadecimal Letters In Lower Case) = %x\n", a);
	printf("Integer Hexadecimal value of 'a' (Hexadecimal Letters In Upper Case) = %X\n\n", a);

	char ch = 'A';
	printf("Character ch = %c\n", ch);
	char str[] = "AstroMediComp's Real Time Rendering Batch";
	printf("String str = %s\n\n", str);

	long num = 30121995L;
	printf("Long Integer = %ld\n\n", num);

	unsigned int b = 7;
	printf("Unsigned Integer 'b' = %u\n\n", b);

	float f_num = 3012.1995f;
	printf("Floating Point Number With Just %%f 'f_num' = %f\n", f_num);
	printf("Floating Point Number With %%4.2f 'f_num' = %4.2f\n", f_num);
	printf("Floating Point Number With %%6.5f 'f_num' = %6.5f\n\n", f_num);

	double d_pi = 3.14159265358979323846;
	printf("Double Precision Floating Point Number Without Exponential = %g\n", d_pi);
	printf("Double Precision Floating Point Number With Exponential (Lower Case) =  %e\n", d_pi);
	printf("Double Precision Floating Point Number With Exponential (Upper Case) = %E\n\n", d_pi);
	printf("Double Hexadecimal Value of 'd_pi' (Hexadecimal Letters In Lower Case) = %a\n", d_pi);
	printf("Double Hexadecimal Value of 'd_pi' (Hexadecimal Letters In Upper Case) = %A\n\n", d_pi);

	printf("=================================================================================================");
	printf("\n\n");

	return 0;
}
