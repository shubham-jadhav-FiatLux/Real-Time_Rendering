#include <stdio.h>

#define MY_PI 3.1415926535897932

#define AMC_STRING "AstroMediComp RTR"

//If First Constant Is Not Assigned A Value, It Is Asssumed To Be 0 i.e. 'SUNDAY' Will Be 0
//And The Rest Of The Constants Are Assigned Consecutive Integer Values From 0 Onwards i.e. 'MONDAY' Will Be 1, 'TUESDAY' Will Be 2, And So On....

// Un-named enum's
enum {
	
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};

enum {

	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

// Named enum's
enum Numbers {

	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE = 5,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN
};

enum boolean {

	TRUE = 1,
	FALSE = 0
};

int main(void) {

	// Local Constant Declaration
	const double epsilon = 0.000001;

	// Code
	printf("\n\n");

	printf("Local Constant Epsilon = %lf\n\n", epsilon);

	printf("SUNDAY Is Day Number %d\n", SUNDAY);
	printf("MONDAY Is Day Number %d\n", MONDAY);
	printf("TUESDAY Is Day Number %d\n", TUESDAY);
	printf("WEDNESDAY Is Day Number %d\n", WEDNESDAY);
	printf("THURSDAY Is Day Number %d\n", THURSDAY);
	printf("FRIDAY Is Day Number %d\n", FRIDAY);
	printf("SATURDAY Is Day Number %d\n\n", SATURDAY);

	printf("One Has Enum Number %d\n", ONE);
	printf("Two Has Enum Number %d\n", TWO);
	printf("Three Has Enum Number %d\n", THREE);
	printf("Four Has Enum Number %d\n", FOUR);
	printf("Five Has Enum Number %d\n", FIVE);
	printf("Six Has Num Number %d\n", SIX);
	printf("Seven Has Enum Number %d\n", SEVEN);
	printf("Eight Has Enum Number %d\n", EIGHT);
	printf("Nine Has Enum Number %d\n", NINE);
	printf("Ten Has Enum Number %d\n\n", TEN);

	printf("January Is Month Number = %d\n", JANUARY);
	printf("February Is Month  NUmber = %d\n", FEBRUARY);
	printf("March Is Month Number = %d\n", MARCH);
	printf("April Is Month Number = %d\n", APRIL);
	printf("May Is Month Number = %d\n", MAY);
	printf("June Is Month Number = %d\n", JUNE);
	printf("July Is Month Number = %d\n", JULY);
	printf("August Is Month Number = %d\n", AUGUST);
	printf("September Is Month Number = %d\n", SEPTEMBER);
	printf("October Is Month Number = %d\n", OCTOBER);
	printf("November Is Month Number = %d\n", NOVEMBER);
	printf("December Is Month Number = %d\n\n", DECEMBER);

	printf("Value Of TRUE Is = %d\n", TRUE);
	printf("Value Of FALSE Is = %d\n\n", FALSE);

	printf("MY_PI Macro Value = %.101f\n\n", MY_PI);
	printf("Area Of Circle Of Radius 2 Units = %f\n\n", (MY_PI * 2.0f * 2.0f)); //PI * r * r = Area Of Circle Of Radius 'r'

	printf("\n\n");

	printf(AMC_STRING);
	printf("\n\n");

	printf("AMC_STRING is : %s\n", AMC_STRING);
	printf("\n\n");

	return 0;
}
