#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_STRING_LENGTH 1024

struct CharacterCount {

	char shubh_ch;
	int shubh_chCount;
} character_and_count[] = {		{'A', 0},
								{'B', 0},
								{'C', 0},
								{'D', 0},
								{'E', 0},
								{'F', 0},
								{'G', 0},
								{'H', 0},
								{'I', 0},
								{'J', 0},
								{'K', 0},
								{'L', 0},
								{'M', 0},
								{'N', 0},
								{'O', 0},
								{'P', 0},
								{'Q', 0},
								{'R', 0},
								{'S', 0},
								{'T', 0},
								{'U', 0},
								{'V', 0},
								{'W', 0},
								{'X', 0},
								{'Y', 0},
								{'Z', 0}	};

#define SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS sizeof(character_and_count)
#define SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS sizeof(character_and_count[0])
#define NUM_ELEMENTS_IN_ARRAY (SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS / SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS)

int main(int argc, char* argv[], char* envp[]) {

	// Variable Declarations
	char str[MAX_STRING_LENGTH];
	int shubh_i, shubh_j, shubh_actualStringLength = 0;

	// Code
	printf("\n\n");
	printf("Enter a string : \n\n");
	gets_s(str, MAX_STRING_LENGTH);
	shubh_actualStringLength = strlen(str);

	printf("\n\n");
	printf("The string you have entered is : \n\n");
	printf("%s\n\n", str);

	for (shubh_i = 0; shubh_i < shubh_actualStringLength; shubh_i++) {
	
		for (shubh_j = 0; shubh_j < NUM_ELEMENTS_IN_ARRAY; shubh_j++) {
		
			str[shubh_i] = toupper(str[shubh_i]);
			if (str[shubh_i] == character_and_count[shubh_j].shubh_ch)
				character_and_count[shubh_j].shubh_chCount++;
		}
	}

	printf("\n\n");
	printf("The number of occurences of all characters from the alphabet are as follows : \n\n");

	for (shubh_i = 0; shubh_i < NUM_ELEMENTS_IN_ARRAY; shubh_i++) {

		printf("Character %c = %d\n", character_and_count[shubh_i].shubh_ch, character_and_count[shubh_i].shubh_chCount);
	}
	printf("\n\n");

	return 0;
}
