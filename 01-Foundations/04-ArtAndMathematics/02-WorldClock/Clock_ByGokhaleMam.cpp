// Header Files
#include <GL/freeglut.h>
#include <math.h>

#define PI 3.14f

// Global Variables
bool bFullScreen = false;

// Entry-point Function
int main(int argc, char* argv[], char* envp[]) {

	// Local Function Declarations
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	// Code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Shubham B Jadhav");

	initialize();

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return 0;
}

void initialize(void) {

	// Code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// BLACK
}

void resize(int width, int height) {

	// Code
	glViewport(0, 0, width, height);
}

void display(void) {

	// Variable Declarations
	float x = 0.0f;
	float y = 0.0f;
	float ir = 0.6f;
	float or = 0.7f;
	float er = 0.1f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);

	glEnd();

	// Cirle
	glBegin(GL_POINTS);

	//glColor3f(1.0f, 1.0f, 0.0f);

	for (float i = 0.0f; i < 7.0f; i += 0.001f) {

		glColor3f(1.0f, 0.0f, 0.0f);		// Red
		glVertex2f(x + ir * cos(i), y + ir * sin(i));	// innner circle
		glVertex2f(x + or * cos(i), y + or * sin(i));	// outer circle

		glColor3f(1.0f, 0.5f, 0.0f);		// Orange
		glVertex2f(0.2f + er * cos(i), 0.2f + er * sin(i));		// right eye
		glVertex2f(-0.2f + er * cos(i), 0.2f + er * sin(i));	// left eye

		glColor3f(1.0f, 1.0f, 1.0f);		// White
		glVertex2f(0.2f + (er - 0.05) * cos(i), 0.15f + (er - 0.05) * sin(i));		// right eye
		glVertex2f(-0.2f + (er - 0.05) * cos(i), 0.15f + (er - 0.05) * sin(i));		// left eye

		glColor3f(0.3f, 0.1f, 0.1f);		// Brown
		glVertex2f(0.2f + (er - 0.08) * cos(i), 0.15f + (er - 0.08) * sin(i));		// right eye
		glVertex2f(-0.2f + (er - 0.08) * cos(i), 0.15f + (er - 0.08) * sin(i));		// left eye

		if ((i >= 4.0f) && (i <= 5.5f)) {

			glColor3f(1.0f, 0.5f, 0.0f);
			glVertex2f(x + 0.3f * cos(i), y + 0.3f * sin(i));		// smile
		}
		
	}
	glEnd();


	// Clock-hand
	glBegin(GL_LINES);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	// Minute hand
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.05, 0.4f);

	glVertex2f(0.0f, 0.0f);
	glVertex2f(-0.05, 0.4f);

	glVertex2f(0.05, 0.4f);
	glVertex2f(0.0f, 0.55f);
	
	glVertex2f(-0.05, 0.4f);
	glVertex2f(0.0f, 0.55f);


	// Hour hand
	glVertex2f(0.0f, 0.0f);
	glVertex2f(-0.3f, 0.05f);

	glVertex2f(0.0f, 0.0f);
	glVertex2f(-0.3f, -0.05f);

	glVertex2f(-0.3, 0.05f);
	glVertex2f(-0.4f, 0.0f);

	glVertex2f(-0.3, -0.05f);
	glVertex2f(-0.4f, 0.0f);

	glEnd();


	// Dial
	glBegin(GL_LINES);

	glColor3f(0.0f, 1.0f, 0.0f);		// Cyan
	//glColor3f(1.0f, 0.0f, 0.0f);
	// 3 O'Clock
	glVertex2f(0.55f, 0.05f);
	glVertex2f(0.55f, -0.05f);

	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.55f, 0.05f);

	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.55f, -0.05f);

	glVertex2f(0.52f, 0.0f);
	glVertex2f(0.55f, 0.0f);

	// 6 O'Clock
	glVertex2f(-0.03f, -0.45f);
	glVertex2f(-0.03f, -0.55f);

	glVertex2f(-0.03f, -0.45f);
	glVertex2f(0.03f, -0.45f);
	
	glVertex2f(-0.03f, -0.55f);
	glVertex2f(0.03f, -0.55f);

	glVertex2f(-0.03f, -0.5f);
	glVertex2f(0.03f, -0.5f);

	glVertex2f(0.03f, -0.5f);
	glVertex2f(0.03f, -0.55f);

	// 9 O'Clock
	glVertex2f(-0.55f, 0.05f);
	glVertex2f(-0.55f, 0.0f);

	glVertex2f(-0.55f, 0.05f);
	glVertex2f(-0.5f, 0.05f);

	glVertex2f(-0.55f, 0.0f);
	glVertex2f(-0.5f, 0.0f);

	glVertex2f(-0.55f, -0.05f);
	glVertex2f(-0.5f, -0.05f);

	glVertex2f(-0.5f, 0.05f);
	glVertex2f(-0.5f, -0.05f);

	glEnd();
	
	glPointSize(2.0f);
	glBegin(GL_POINTS);

	// Remaining points
	float dr = 0.55f;
	for (int i = 0; i < 360; i += 30) {
	
		if (i == 0 || i == 180 || i == 270)
			continue;
	
		glVertex2f(dr*cos((i*PI)/180.0) , dr* sin((i* PI) / 180.0));
	}

	glEnd();


	// Arms
	glPointSize(1.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.5f, 0.0f);		// Orange
	// Left arm
	glVertex2f(-0.67f, 0.2f);
	glVertex2f(-0.8f, 0.09f);

	glVertex2f(-0.66f, 0.25f);
	glVertex2f(-0.828f, 0.11f);


	glVertex2f(-0.7f, -0.2f);
	glVertex2f(-0.8f, -0.09f);

	glVertex2f(-0.73f, -0.2f);
	glVertex2f(-0.83f, -0.10f);


	// right hand
	glVertex2f(0.67f, 0.2f);
	glVertex2f(0.8f, 0.09f);

	glVertex2f(0.66f, 0.25f);
	glVertex2f(0.8f, 0.14f);


	glVertex2f(0.85f, 0.15f);
	glVertex2f(0.9f, 0.35f);

	glVertex2f(0.88f, 0.12f);
	glVertex2f(0.92f, 0.34f);

	glEnd();

	// Palm
	glBegin(GL_POINTS);

	for (float i = 0; i < 360; i += 0.1) {

		if (i >= 155 && i <= 205){

			glVertex2f(-0.6f + 0.22 * cos((i * PI) / 180.0f), -0.0f + 0.22 * sin((i * PI) / 180.0f));
			glVertex2f(-0.6f + 0.25 * cos((i * PI) / 180.0f), -0.0f + 0.25 * sin((i * PI) / 180.0f));
		}
		glVertex2f(-0.71f + 0.05*cos((i*PI)/180.0f), -0.25f + 0.05 * sin((i * PI) / 180.0f));

		if (i >= 270 && i <= 300) {

			glVertex2f(0.8f + 0.11 * cos((i * PI) / 180.0f), 0.25f + 0.11 * sin((i * PI) / 180.0f));
			glVertex2f(0.8f + 0.16 * cos((i * PI) / 180.0f), 0.25f + 0.16 * sin((i * PI) / 180.0f));
		}
		glVertex2f(0.92f + 0.05 * cos((i * PI) / 180.0f), 0.4f + 0.05 * sin((i * PI) / 180.0f));
	}
	
	glEnd();


	// Top
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex2f(0.2, 0.8f);
	glVertex2f(0.7f, 0.6f);

	glVertex2f(-0.2, 0.8f);
	glVertex2f(-0.7f, 0.6f);

	glVertex2f(0.7f * cos((60*PI)/180.0f), 0.7f * sin((60 * PI) / 180.0f));
	glVertex2f(0.8f * cos((60 * PI) / 180.0f), 0.83f * sin((60 * PI) / 180.0f));

	glVertex2f(0.8f * cos((60 * PI) / 180.0f), 0.68f * sin((60 * PI) / 180.0f));
	glVertex2f(0.9f * cos((60 * PI) / 180.0f), 0.81f * sin((60 * PI) / 180.0f));


	glVertex2f(-0.7f * cos((60 * PI) / 180.0f), 0.7f * sin((60 * PI) / 180.0f));
	glVertex2f(-0.8f * cos((60 * PI) / 180.0f), 0.83f * sin((60 * PI) / 180.0f));

	glVertex2f(-0.8f * cos((60 * PI) / 180.0f), 0.68f * sin((60 * PI) / 180.0f));
	glVertex2f(-0.9f * cos((60 * PI) / 180.0f), 0.81f * sin((60 * PI) / 180.0f));

	glEnd();

	glBegin(GL_POINTS);
	
	// Middle
	glVertex2f((0.7f + 0.2f) / 2.0f, (0.6f + 0.8f) / 2.0f);
	float m1 = (0.7f + 0.2f) / 2.0f;
	float m2 = (0.6f + 0.8f) / 2.0f;
	
	// Semicircle
	for (float i = -0.4f; i < 2.78f; i += 0.001f) {
	
		glVertex2f(m1 + (((0.7f - 0.2f) / 2.0f)+0.02f) * cos(i), m2 + (((0.7f - 0.2f) / 2.0f)+0.02f) * sin(i));
	}

	for (float i = -3.5f; i > -6.68f; i -= 0.001f) {

		glVertex2f(-m1 - (((0.7f - 0.2f) / 2.0f)+0.02f) * cos(i), m2 + (((0.7f - 0.2f) / 2.0f)+0.02f) * sin(i));
	}

	glEnd();


	// Legs
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.5f, 0.0f);		// Orange
	// Left leg
	glVertex2f(-0.06f, -0.7f);
	glVertex2f(-0.08f, -0.8f);
	
	glVertex2f(-0.14f, -0.69f);
	glVertex2f(-0.12f, -0.8f);

	glVertex2f(-0.12f, -0.8f);
	glVertex2f(-0.08f, -0.8f);

	// Right leg
	glVertex2f(0.06f, -0.7f);
	glVertex2f(0.08f, -0.8f);

	glVertex2f(0.14f, -0.69f);
	glVertex2f(0.12f, -0.8f);

	glVertex2f(0.12f, -0.8f);
	glVertex2f(0.08f, -0.8f);

	glEnd();


	// Shoes
	glBegin(GL_POINTS);

	glColor3f(0.0f, 0.5f, 0.1f);
	float sr = 0.07f;
	for (float i = 0; i < 360; i += 0.1) {

		if (i >= 60 && i <= 120)
			continue;

		// Left sole 
		if (i <= 215 || i >= 290)
			glVertex2f(-(0.12f + 0.08) / 2.0f + sr * cos((i * PI) / 180.0), -(0.8) + sr * sin((i * PI) / 180.0));
		
		// Right sole
		if (i <= 255 || i >= 330)
			glVertex2f((0.12f + 0.08) / 2.0f + sr * cos((i * PI) / 180.0), -(0.8) + sr * sin((i * PI) / 180.0));
	
		// Left toe
		if (i <= 30 || i >= 120)
			glVertex2f(-(0.03 + (0.12f + 0.08) / 2.0f) + (sr-0.01) * cos((i * PI) / 180.0), -(0.79 + (0.12f + 0.08) / 2.0f) + (sr-0.01) * sin((i * PI) / 180.0));
		
		// Right toe
		if (i <= 60 || i >= 155)
			glVertex2f((0.03 + (0.12f + 0.08) / 2.0f) + (sr - 0.01) * cos((i * PI) / 180.0), -(0.79 + (0.12f + 0.08) / 2.0f) + (sr - 0.01) * sin((i * PI) / 180.0));

	}

	glEnd();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {

	// Code
	switch (key) {
	
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if (bFullScreen == false) {
		
			glutFullScreen();
			bFullScreen = true;
		}
		else {
		
			glutLeaveFullScreen();
			bFullScreen = false;
		}
		break;

	default :
		break;
	}
}

void mouse(int button, int state, int x, int y) {

	// Code
	switch (button) {
	
	case GLUT_LEFT_BUTTON:
		glutLeaveMainLoop();
		break;

	default :
		break;
	}
}

void uninitialize(void) {

	// Code
}
