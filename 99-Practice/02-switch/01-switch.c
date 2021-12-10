// Program for Finding Month Using Switch

int main(void) {

	// Variable Declaration
	unsigned int sj_month;

	// Code
	printf("Enter Month Number : \n\n");
	scanf("%u", &sj_month);

	switch (sj_month) {
	
	case 1:
		printf("January\n");
		break;

	case 2:
		printf("February\n")
		break;

	case 3:
		printf("March\n");
		break;

	case 4:
		printf("April\n");
		break;

	case 5:
		printf("May\n");
		break;

	case 6:
		printf("June\n");
		break;

	case 7:
		printf("July\n");
		break;

	case 8:
		printf("Aug\n");
		break;

	case 9:
		printf("Sep"\n");
		break;

	case 10:
		printf("Oct\n");
		break;

	case 11:
		printf("Nov\n");
		break;

	case 12:
		printf("Dec\n");
		break;

	default :
		printf("INVALID MONTH !\n\n");
		break;
	}

	return 0;
}
