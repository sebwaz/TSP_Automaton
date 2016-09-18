#include <iostream>
#include <time.h>
#include <glut.h>
using namespace std;

/////////////
// GLOBALS //
/////////////

/* TSP SPEC */
static const int S_GRID = 50; // assume square grid for now
static const int NUM_PT = 6;  // TODO: colorization currently only handles 6 points 
static int GRID[S_GRID][S_GRID];
static GLfloat unit_span = 0.3 / S_GRID;



/////////////
// GL FXNS //
/////////////

// draw function called by glutMainLoop()
void Draw()
{
	/* SPECIFY COLOR */
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	/* DRAW MAP */
	for (int i = 0; i < S_GRID; i++)
	{
		for (int j = 0; j < S_GRID; j++)
		{
			// color based on grid value
			cout << GRID[i][j];
			switch(GRID[i][j])
			{ 
				case 0: glColor3f(1.0, 0.0, 0.0); break;
				case 1: glColor3f(0.0, 1.0, 0.0); break;
				case 2: glColor3f(0.0, 0.0, 1.0); break;
				case 3: glColor3f(1.0, 1.0, 0.0); break;
				case 4: glColor3f(0.0, 1.0, 1.0); break;
				case 5: glColor3f(1.0, 0.0, 1.0); break;
			}

			// draw unit
			glBegin(GL_POLYGON);
			glVertex3f(0.1 + unit_span*(i),   0.35 + unit_span*(j),   0.0);
			glVertex3f(0.1 + unit_span*(i+1), 0.35 + unit_span*(j),   0.0);
			glVertex3f(0.1 + unit_span*(i),   0.35 + unit_span*(j+1), 0.0);
			glVertex3f(0.1 + unit_span*(i+1), 0.35 + unit_span*(j+1), 0.0);
			glEnd();
		}
	}
	glFlush();
}

// called when Draw() returns
void Idle()
{
	// calls Draw()
	glutPostRedisplay();
}


// called before glutMainLoop() to specify properties
void Initialize()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}



//////////
// MAIN //
//////////

int main(int iArgc, char** cppArgv)
{
	// initialize map with random values
	srand(time(NULL));
	for (int i = 0; i < S_GRID; i++)
	{
		for (int j = 0; j < S_GRID; j++)
		{
			GRID[i][j] = rand() % NUM_PT;
		}
	}
	
	// run glut
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("TSP Automaton");
	Initialize();
	glutDisplayFunc(Draw);
	glutIdleFunc(Idle);
	glutMainLoop();
	return 0;
}