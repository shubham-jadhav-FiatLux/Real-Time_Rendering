//Header Files
#include <GL/freeglut.h>

//Global Variables
bool bFullScreen = false;

//Entry-point Function
int main(int argc, char* argv[]) {

	//Local Function Declarations
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	//Code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Bhai ka Bday !!");

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

	//Code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void resize(int width, int height) {

	//Code
	glViewport(0, 0, width, height);
}

void display(void) {

	//Code
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_LINES);

	// H
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	glVertex2f(-0.8f, 0.8f);		// HAPPY Start
	glVertex2f(-0.8f, 0.2f);

	glVertex2f(-0.71f, 0.8f);
	glVertex2f(-0.71f, 0.2f);

	glVertex2f(-0.8f, 0.6f);
	glVertex2f(-0.71f, 0.6f);


	// A
	glVertex2f(-0.69f, 0.8f);
	glVertex2f(-0.69f, 0.2f);

	glVertex2f(-0.61f, 0.8f);
	glVertex2f(-0.61f, 0.2f);

	glVertex2f(-0.69f, 0.6f);
	glVertex2f(-0.61f, 0.6f);

	glVertex2f(-0.69f, 0.8f);
	glVertex2f(-0.61f, 0.8f);


	// P
	glVertex2f(-0.59f, 0.8f);
	glVertex2f(-0.59f, 0.2f);

	glVertex2f(-0.51f, 0.8f);
	glVertex2f(-0.51f, 0.6f);

	glVertex2f(-0.59f, 0.6f);
	glVertex2f(-0.51f, 0.6f);

	glVertex2f(-0.59f, 0.8f);
	glVertex2f(-0.51f, 0.8f);


	// P
	glVertex2f(-0.49f, 0.8f);
	glVertex2f(-0.49f, 0.2f);

	glVertex2f(-0.41f, 0.8f);
	glVertex2f(-0.41f, 0.6f);

	glVertex2f(-0.49f, 0.6f);
	glVertex2f(-0.41f, 0.6f);

	glVertex2f(-0.49f, 0.8f);
	glVertex2f(-0.41f, 0.8f);


	// Y
	glVertex2f(-0.39f, 0.8f);
	glVertex2f(-0.35f, 0.6f);

	glVertex2f(-0.31f, 0.8f);
	glVertex2f(-0.35f, 0.6f);

	glVertex2f(-0.35f, 0.6f);
	glVertex2f(-0.35f, 0.2f);		// HAPPY Completed



	// B
	glVertex2f(0.0f, 0.8f);			// BIRTHDAY Start
	glVertex2f(0.0f, 0.2f);

	glVertex2f(0.09f, 0.8f);
	glVertex2f(0.09f, 0.2f);

	glVertex2f(0.0f, 0.8f);
	glVertex2f(0.09f, 0.8f);

	glVertex2f(0.0f, 0.6f);
	glVertex2f(0.09f, 0.6f);

	glVertex2f(0.0f, 0.2f);
	glVertex2f(0.09f, 0.2f);


	// I
	glVertex2f(0.15f, 0.8f);
	glVertex2f(0.15f, 0.2f);

	glVertex2f(0.11f, 0.8f);
	glVertex2f(0.19f, 0.8f);

	glVertex2f(0.11f, 0.2f);
	glVertex2f(0.19f, 0.2f);


	// R
	glVertex2f(0.21f, 0.8f);
	glVertex2f(0.21f, 0.2f);

	glVertex2f(0.29f, 0.8f);
	glVertex2f(0.29f, 0.6f);

	glVertex2f(0.25f, 0.6f);
	glVertex2f(0.29f, 0.2f);

	glVertex2f(0.21f, 0.6f);
	glVertex2f(0.29f, 0.6f);

	glVertex2f(0.21f, 0.8f);
	glVertex2f(0.29f, 0.8f);


	// T
	glVertex2f(0.35f, 0.8f);
	glVertex2f(0.35f, 0.2f);

	glVertex2f(0.31f, 0.8f);
	glVertex2f(0.39f, 0.8f);


	// H
	glVertex2f(0.41f, 0.8f);
	glVertex2f(0.41f, 0.2f);

	glVertex2f(0.49f, 0.8f);
	glVertex2f(0.49f, 0.2f);

	glVertex2f(0.41f, 0.6f);
	glVertex2f(0.49f, 0.6f);


	// D
	glVertex2f(0.51f, 0.8f);		
	glVertex2f(0.51f, 0.2f);

	glVertex2f(0.59f, 0.7f);
	glVertex2f(0.59f, 0.3f);

	glVertex2f(0.51f, 0.8f);
	glVertex2f(0.59f, 0.7f);
			
	glVertex2f(0.51f, 0.2f);
	glVertex2f(0.59f, 0.3f);


	// A
	glVertex2f(0.61f, 0.8f);
	glVertex2f(0.61f, 0.2f);

	glVertex2f(0.69f, 0.8f);
	glVertex2f(0.69f, 0.2f);

	glVertex2f(0.61f, 0.6f);
	glVertex2f(0.69f, 0.6f);

	glVertex2f(0.61f, 0.8f);
	glVertex2f(0.69f, 0.8f);


	// Y
	glVertex2f(0.71f, 0.8f);
	glVertex2f(0.75f, 0.6f);

	glVertex2f(0.79f, 0.8f);
	glVertex2f(0.75f, 0.6f);

	glVertex2f(0.75f, 0.6f);
	glVertex2f(0.75f, 0.2f);			// BIRTHDAY Completed


	// Y
	glColor3f(0.0f, 1.0f, 0.0f);

	glVertex2f(-0.25f, -0.8f);			// YOGESH Start
	glVertex2f(-0.25f, -0.4f);

	glVertex2f(-0.3f, -0.2f);
	glVertex2f(-0.25f, -0.4f);

	glVertex2f(-0.21f, -0.2f);
	glVertex2f(-0.25f, -0.4f);


	// O
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex2f(-0.19f, -0.8f);		
	glVertex2f(-0.19f, -0.2f);

	glVertex2f(-0.11f, -0.8f);
	glVertex2f(-0.11f, -0.2f);

	glVertex2f(-0.19f, -0.8f);
	glVertex2f(-0.11f, -0.8f);

	glVertex2f(-0.19f, -0.2f);
	glVertex2f(-0.11f, -0.2f);


	// G
	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex2f(-0.09f, -0.8f);
	glVertex2f(-0.09f, -0.2f);

	glVertex2f(-0.01f, -0.2f);
	glVertex2f(-0.01f, -0.4f);

	glVertex2f(-0.01f, -0.6f);
	glVertex2f(-0.05f, -0.6f);

	glVertex2f(-0.01f, -0.6f);
	glVertex2f(-0.01f, -0.8f);

	glVertex2f(-0.09f, -0.8f);
	glVertex2f(-0.01f, -0.8f);

	glVertex2f(-0.09f, -0.2f);
	glVertex2f(-0.01f, -0.2f);


	// E
	glColor3f(2.0f, 0.5f, 1.0f);

	glVertex2f(0.01f, -0.8f);
	glVertex2f(0.01f, -0.2f);

	glVertex2f(0.01f, -0.2f);
	glVertex2f(0.09f, -0.2f);

	glVertex2f(0.01f, -0.4f);
	glVertex2f(0.09f, -0.4f);

	glVertex2f(0.01f, -0.8f);
	glVertex2f(0.09f, -0.8f);


	// S
	glColor4f(1.0f, 1.0f, 0.0f, 0.0f);

	glVertex2f(0.11f, -0.2f);
	glVertex2f(0.11f, -0.4f);

	glVertex2f(0.19f, -0.4f);
	glVertex2f(0.19f, -0.8f);

	glVertex2f(0.11f, -0.2f);
	glVertex2f(0.19f, -0.2f);

	glVertex2f(0.11f, -0.4f);
	glVertex2f(0.19f, -0.4f);

	glVertex2f(0.11f, -0.8f);
	glVertex2f(0.19f, -0.8f);


	// H
	glColor3f(1.0f, 0.5f, 0.0f);

	glVertex2f(0.21f, -0.2f);
	glVertex2f(0.21f, -0.8f);

	glVertex2f(0.29f, -0.2f);
	glVertex2f(0.29f, -0.8f);

	glVertex2f(0.21f, -0.4f);
	glVertex2f(0.29f, -0.4f);		// YOGESH Completed


	glEnd();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {

	//Code
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

	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {

	//Code
	switch (button) {
	
	case GLUT_LEFT_BUTTON:
		glutLeaveMainLoop();
		break;

	default:
		break;
	}
}

void uninitialize(void) {

	//Code
}
