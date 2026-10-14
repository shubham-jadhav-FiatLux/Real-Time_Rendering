// Header Files
#include <GL/glut.h>
#include <stdlib.h>

void display(void)
{
	// Code
	// Clear All Pixels
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw White Rectangle
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
		glVertex3f(0.25, 0.25, 0.0);
		glVertex3f(0.75, 0.25, 0.0);
		glVertex3f(0.75, 0.75, 0.0);
		glVertex3f(0.25, 0.75, 0.0);
	glEnd();

	glFlush();
}

void init(void)
{
	// Code
	// Select Clearing (Background) Color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Initialize Viewing Values
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv)
{
	// Code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello");
	init();
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}