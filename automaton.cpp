#include <iostream>
#include <glut.h>
using namespace std;

int atm_grid;

void InitRendering()
{
	glClearColor(0, 0, 0, 0);
}

void Display()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	glBegin(GL_LINES);
	glVertex3f(0, 0, -5);
	glVertex3f(2, 2, -(atm_grid++)%8);
	glEnd();

	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 1.0, 200.0);
}

int main()
{

	atm_grid = 4;

	/* Initialize GLUT */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);

	/* Create windows */
	glutCreateWindow("TSP Automaton"); //takes name of window as arg
	InitRendering();

	/* Handler fxns */
	glutDisplayFunc(Display); //specify fxn called to draw window contents
	glutReshapeFunc(Reshape); //specify fxn called when window is resized

	/* Main loop */
	glutMainLoop(); //calls display and reshape fxns
	return 0;
}