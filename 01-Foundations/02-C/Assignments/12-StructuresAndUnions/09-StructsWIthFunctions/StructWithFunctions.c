#include <stdio.h>

// Defining Struct
struct MyData {

	int shubh_i;
	float shubh_f;
	double shubh_d;
	char shubh_c;
};

int main(int argc, char* argv[], char* envp[]) {

	// Function Prototype
	struct MyData AddStructMembers(struct MyData, struct MyData, struct MyData);

	// Variable Declarations
	struct MyData data1, data2, data3, answer_data;

	// Code
	// ****** data 1 *****
	printf("\n\n\n");
	printf("******** DATA 1 *********\n\n");
	printf("Enter Integer Value For 'i' Of 'struct MyData data1' : ");
	scanf("%d", &data1.shubh_i);

	printf("\n\n");
	printf("Enter Floating-point Value For 'f' Of 'struct MyData data1' : ");
	scanf("%f", &data1.shubh_f);

	printf("\n\n");
	printf("Enter Double Value For 'd' Of 'struct MyData data1' : ");
	scanf("%lf", &data1.shubh_d);

	printf("\n\n");
	printf("Enter Character Value For 'c' Of 'struct MyData data1' : ");
	data1.shubh_c = getch();
	printf("%c", data1.shubh_c);


	// ****** data 2 *****
	printf("\n\n\n");
	printf("******** DATA 2 *********\n\n");
	printf("Enter Integer Value For 'i' Of 'struct MyData data2' : ");
	scanf("%d", &data2.shubh_i);

	printf("\n\n");
	printf("Enter Floating-point Value For 'f' Of 'struct MyData data2' : ");
	scanf("%f", &data2.shubh_f);

	printf("\n\n");
	printf("Enter Double Value For 'd' Of 'struct MyData data2' : ");
	scanf("%lf", &data2.shubh_d);

	printf("\n\n");
	printf("Enter Character Value For 'c' Of 'struct MyData data2' : ");
	data2.shubh_c = getch();
	printf("%c", data2.shubh_c);
	
	
	// ****** data 3 *****
	printf("\n\n\n");
	printf("******** DATA 3 *********\n\n");
	printf("Enter Integer Value For 'i' Of 'struct MyData data3' : ");
	scanf("%d", &data3.shubh_i);

	printf("\n\n");
	printf("Enter Floating-point Value For 'f' Of 'struct MyData data3' : ");
	scanf("%f", &data3.shubh_f);

	printf("\n\n");
	printf("Enter Double Value For 'd' Of 'struct MyData data3' : ");
	scanf("%lf", &data3.shubh_d);

	printf("\n\n");
	printf("Enter Character Value For 'c' Of 'struct MyData data3' : ");
	data3.shubh_c = getch();
	printf("%c", data3.shubh_c);


	answer_data = AddStructMembers(data1, data2, data3);

	printf("\n\n\n");
	printf("******** ANSWER *********\n\n");
	printf("answer_data.shubh_i = %d\n", answer_data.shubh_i);
	printf("answer_data.shubh_f = %f\n", answer_data.shubh_f);
	printf("answer_data.shubh_d = %lf\n\n", answer_data.shubh_d);

	answer_data.shubh_c = data1.shubh_c;
	printf("answer_data.shubh_c (from data1) = %c\n\n", answer_data.shubh_c);

	answer_data.shubh_c = data2.shubh_c;
	printf("answer_data.shubh_c (from data2) = %c\n\n", answer_data.shubh_c);

	answer_data.shubh_c = data3.shubh_c;
	printf("answer_data.shubh_c (from data3) = %c\n\n", answer_data.shubh_c);

	return 0;
}

struct MyData AddStructMembers(struct MyData md_one, struct MyData md_two, struct MyData md_three) {

	// Variable Declarations
	struct MyData answer;

	// Code
	answer.shubh_i = md_one.shubh_i + md_two.shubh_i + md_three.shubh_i;
	answer.shubh_f = md_one.shubh_f + md_two.shubh_f + md_three.shubh_f;
	answer.shubh_d = md_one.shubh_d + md_two.shubh_d + md_three.shubh_d;

	return answer;
}
