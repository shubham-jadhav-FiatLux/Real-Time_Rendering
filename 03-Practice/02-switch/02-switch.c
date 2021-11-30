// Program to check whether alphabet is Vowel or Consonant

#include <stdio.h>

int main(void) {

	// Variable Declaration
	char sj_alpha;

	// Code
	printf("Enter Alphabet: \n\n");
	scanf("%c", &sj_alpha);

	switch (sj_alpha) {
	
	case 'A':
	case 'a':
	case 'E':
	case 'e':
	case 'I':
	case 'i':
	case 'O':
	case 'o':
	case 'U':
	case 'u':
		printf("It Is A Vowel\n\n");
		break;

	default :
		printf("It Is A Consonant\n\n");
		break;
	}

	return 0;
}

