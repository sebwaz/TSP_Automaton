#include "automaton.h"
using namespace std;



/////////////
// GLOBALS //
/////////////

/* FOR GRID DRAW */
static GLfloat UNIT_SPAN = 0.3 / get_grid_w(); // assume square
static bool done = false;



/////////////
// GL FXNS //
/////////////

static void timerCallback(int value) {
	if (!done) { done = !radiate(); print_neighbors_all(); }
	glutPostRedisplay();
}

// draw function called by glutMainLoop()
void Draw()
{
	/* SPECIFY COLOR */
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	/* DRAW MAP */
	for (int i = 0; i < get_grid_w(); i++)
	{
		for (int j = 0; j < get_grid_h(); j++)
		{
			// TODO: colorization currently only handles 7 points
			// cout << GRID[i][j];
			switch(get_grid_state()[i][j])
			{
				// colorize based on grid value
				case 0:  glColor3f(0.2, 0.2, 0.2); break; // indicates blank space
				case 1:  glColor3f(1.0, 0.0, 0.0); break; // 1 corresponds to 1st point in grid
				case 2:  glColor3f(0.0, 1.0, 0.0); break; // 2 corresponds to 2nd point in grid
				case 3:  glColor3f(0.0, 0.0, 1.0); break; // etc.
				case 4:  glColor3f(1.0, 1.0, 0.0); break;
				case 5:  glColor3f(0.0, 1.0, 1.0); break;
				case 6:  glColor3f(1.0, 0.0, 1.0); break;
				case 7:  glColor3f(1.0, 1.0, 1.0); break;
				default: glColor3f(0.2, 0.2, 0.2); break;
			}

			// draw unit
			glBegin(GL_POLYGON);
			glVertex3f(0.1 + UNIT_SPAN*(i),   0.35 + UNIT_SPAN*(j),   0.0);
			glVertex3f(0.1 + UNIT_SPAN*(i+1), 0.35 + UNIT_SPAN*(j),   0.0);
			glVertex3f(0.1 + UNIT_SPAN*(i+1), 0.35 + UNIT_SPAN*(j+1), 0.0);
			glVertex3f(0.1 + UNIT_SPAN*(i),   0.35 + UNIT_SPAN*(j+1), 0.0);
			glEnd();
		}
	}
	glFlush();
	glutTimerFunc(100, timerCallback, NULL);
}

// called when Draw() returns
void Idle()
{
	// calls Draw()
	// radiate();
	// glutPostRedisplay();
	// currently relying on glutTimerFunc in Draw()
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
	// allocate grid
	InitTSP();
	
	// run visualization
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