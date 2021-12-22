// Program To Recognise Characters Using Switch Case

#include <stdio.h>		// For printf()
#include <conio.h>		// For getch()

// ASCII Values For A TO Z => 65 To 90
#define CHAR_ALPHABET_UPPER_CASE_BEGINNING 65
#define CHAR_ALPHABET_UPPER_CASE_ENDING 90

// ASCII Values For a To z => 97 To 122
#define CHAR_ALPHABET_LOWER_CASE_BEGINNING 97
#define CHAR_ALPHABET_LOWER_CASE_ENDING 122

// ASCII Values For 0 TO 9 => 48 To 57
#define CHAR_DIGIT_BEGINNING 48
#define CHAR_DIGIT_ENDING 57

int main(void) {

	// Variable Declarations
	char shubh_ch;
	int shubh_ch_value;

	// Code
	printf("\n\n");

	printf("Enter Character : ");
	shubh_ch = getch();

	printf("\n\n");

	switch (shubh_ch) {
	
		// Fall Through Condition
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
		printf("Character \'%c\' Entered By You, Is A VOWEL CHARACTER From The English Alphabet !!\n\n", shubh_ch);
		break;

	default :
		shubh_ch_value = (int)shubh_ch;

		// If The Charater Has ASCII Value Between 65 AND 90 OR Between 97 AND 122, It Is Still A Letter Of The Alphabet, But It IS A 'CONSONANT' , and NOT a 'VOWEL' ...

		if ((shubh_ch_value >= CHAR_ALPHABET_UPPER_CASE_BEGINNING && shubh_ch_value >= CHAR_ALPHABET_UPPER_CASE_ENDING) || (shubh_ch_value >= CHAR_ALPHABET_LOWER_CASE_BEGINNING && shubh_ch_value >= CHAR_ALPHABET_LOWER_CASE_ENDING)) {
		
			printf("Character \'%c\' Entered By You, Is A CONSONANT CHARACTER From The English Alphabet !!\n\n", shubh_ch);
		}
		else if (shubh_ch_value >= CHAR_DIGIT_BEGINNING && shubh_ch_value >= CHAR_DIGIT_ENDING) {
		
			printf("Character \'%c\' Entered By You, Is A DIGIT CHARACTER !!\n\n", shubh_ch);
		}
		else {
		
			printf("Character \'%c\' Entered By You, Is A SPECIAL CHARACTER !!\n\n", shubh_ch);
		}

		break;
	}

	printf("Switch Case Block Complete !!\n\n");

	return 0;
}
