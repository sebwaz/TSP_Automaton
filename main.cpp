#include "automaton.h"
#include "rgb.h"
using namespace std;



/////////////
// GLOBALS //
/////////////

/* FOR GRID DRAW */
static GLfloat UNIT_STRETCH = 0.01;               // for extending units on connection map to be visible @ small unit size
static GLfloat UNIT_SPAN    = 0.3 / get_grid_w(); // assume square
static bool    done         = false;

/* FOR COLORS */
static float** colors;



/////////////
// GL FXNS //
/////////////

static void timerCallback(int value) {
	if (!done) { done = !radiate(); print_neighbors_all(); }
	else	   { assign_links(); }
	glutPostRedisplay();
}

// draw function called by glutMainLoop()
void Draw()
{
	/* SPECIFY COLOR */
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	/* DRAW RADIATION MAP */
	for (int i = 0; i < get_grid_w(); i++) { for (int j = 0; j < get_grid_h(); j++) {
		// TODO: colorization currently only handles 7 points
		// cout << GRID[i][j];
		int state = get_grid_state()[i][j] - 1;
		switch(state + 1)
		{
			// colorize based on grid value
			case -1: glColor3f(0.2, 0.2, 0.2); break;
			case  0: glColor3f(0.2, 0.2, 0.2); break; // indicates blank space
			default: glColor3f(colors[state][0], colors[state][1], colors[state][2]); break;
		}

		// draw unit
		glBegin(GL_POLYGON);
		glVertex3f(0.1 + UNIT_SPAN*(i),   0.35 + UNIT_SPAN*(j),   0.0);
		glVertex3f(0.1 + UNIT_SPAN*(i+1), 0.35 + UNIT_SPAN*(j),   0.0);
		glVertex3f(0.1 + UNIT_SPAN*(i+1), 0.35 + UNIT_SPAN*(j+1), 0.0);
		glVertex3f(0.1 + UNIT_SPAN*(i),   0.35 + UNIT_SPAN*(j+1), 0.0);
		glEnd();
	}}

	/* DRAW CONNECTION MAP */

	// BASE
	glColor3f(0.2, 0.2, 0.2);
	for (int i = 0; i < get_grid_w(); i++) { for (int j = 0; j < get_grid_h(); j++) {
		glBegin(GL_POLYGON);
		glVertex3f(0.6 + UNIT_SPAN*(i),     0.35 + UNIT_SPAN*(j),     0.0);
		glVertex3f(0.6 + UNIT_SPAN*(i + 1), 0.35 + UNIT_SPAN*(j),     0.0);
		glVertex3f(0.6 + UNIT_SPAN*(i + 1), 0.35 + UNIT_SPAN*(j + 1), 0.0);
		glVertex3f(0.6 + UNIT_SPAN*(i),     0.35 + UNIT_SPAN*(j + 1), 0.0);
		glEnd();
	}}

	// POINTS + CONNECTIONS
	for (int i = 0; i < get_num_atmn(); i++)
	{
		Automaton* point = get_arr_atmn(i);
		glColor3f(colors[i][0], colors[i][1], colors[i][2]);

		int x = point->get_xy()[0];
		int y = point->get_xy()[1];

		// PRINT POINT
		glBegin(GL_POLYGON);
		glVertex3f(0.6 + UNIT_SPAN*(x)-UNIT_STRETCH,     0.35 + UNIT_SPAN*(y)-UNIT_STRETCH,     1.0);
		glVertex3f(0.6 + UNIT_SPAN*(x + 1)+UNIT_STRETCH, 0.35 + UNIT_SPAN*(y)-UNIT_STRETCH,     1.0);
		glVertex3f(0.6 + UNIT_SPAN*(x + 1)+UNIT_STRETCH, 0.35 + UNIT_SPAN*(y + 1)+UNIT_STRETCH, 1.0);
		glVertex3f(0.6 + UNIT_SPAN*(x)-UNIT_STRETCH,     0.35 + UNIT_SPAN*(y + 1)+UNIT_STRETCH, 1.0);
		glEnd();

		// PRINT CXNS
		glLineWidth(0.01);
		glColor3f(0.25, 0.25, 0.25);
		Node* iter = point->get_neighbors();

		// TODO: eliminate switch redundancy
		// TODO: verify this works because you did it drunk
		if           (iter         == NULL) {}
		else { while (iter->n_next != NULL) {
			   // draw line to final cxn
			   int n_offX;
			   int n_offY;
			   switch (iter->n_origin)
			   {
					case 0: n_offX = -GRID_W; n_offY =  GRID_H; break;
					case 1: n_offX = -GRID_W; n_offY = 0;       break;
					case 2: n_offX = -GRID_W; n_offY = -GRID_H; break;
					case 3: n_offX = 0;       n_offY =  GRID_H; break;
					case 4: n_offX = 0;       n_offY = 0;       break;
					case 5: n_offX = 0;       n_offY = -GRID_H; break;
					case 6: n_offX =  GRID_W; n_offY =  GRID_H; break;
					case 7: n_offX =  GRID_W; n_offY = 0;       break;
					case 8: n_offX =  GRID_W; n_offY = -GRID_H; break;
					default: break;
			   }
			   glBegin(GL_LINES);
			   glVertex3f(0.6 + UNIT_SPAN*(x + 0.5),                                  0.35 + UNIT_SPAN*(y + 0.5),                                  0.0);
			   glVertex3f(0.6 + UNIT_SPAN*(iter->n_atmn->get_xy()[0] + n_offX + 0.5), 0.35 + UNIT_SPAN*(iter->n_atmn->get_xy()[1] + n_offY + 0.5), 0.0);
			   glEnd();

			   // go to next cxn
			   iter = iter->n_next; }
		
			   // draw line to cxn
			   int n_offX;
			   int n_offY;
			   switch (iter->n_origin)
			   {
					case 0: n_offX = -GRID_W; n_offY =  GRID_H; break;
					case 1: n_offX = -GRID_W; n_offY = 0;       break;
					case 2: n_offX = -GRID_W; n_offY = -GRID_H; break;
					case 3: n_offX = 0;       n_offY =  GRID_H; break;
					case 4: n_offX = 0;       n_offY = 0;       break;
					case 5: n_offX = 0;       n_offY = -GRID_H; break;
					case 6: n_offX =  GRID_W; n_offY =  GRID_H; break;
					case 7: n_offX =  GRID_W; n_offY = 0;       break;
					case 8: n_offX =  GRID_W; n_offY = -GRID_H; break;
					default: break;
			   }
			   glBegin(GL_LINES);
			   glVertex3f(0.6 + UNIT_SPAN*(x + 0.5),                                  0.35 + UNIT_SPAN*(y + 0.5),                                  0.0);
			   glVertex3f(0.6 + UNIT_SPAN*(iter->n_atmn->get_xy()[0] + n_offX + 0.5), 0.35 + UNIT_SPAN*(iter->n_atmn->get_xy()[1] + n_offY + 0.5), 0.0);
			   glEnd();
		}

		// PRINT LINKS
		// TODO: verify this works properly, reduce switch redundancy
		// TODO: figure out why link lines are sometimes drawn underneath neighbor/cxn lines
		glLineWidth(3);
		glColor3f(1.0, 1.0, 1.0);
		for (int j = 0; j < 2 && point->get_links()[j] != NULL; j++)
		{
			Node* link = point->get_links()[j];
			int l_offX;
			int l_offY;
			switch (link->n_origin)
			{
				case 0: l_offX = -GRID_W; l_offY =  GRID_H; break;
				case 1: l_offX = -GRID_W; l_offY = 0;       break;
				case 2: l_offX = -GRID_W; l_offY = -GRID_H; break;
				case 3: l_offX = 0;       l_offY =  GRID_H; break;
				case 4: l_offX = 0;       l_offY = 0;       break;
				case 5: l_offX = 0;       l_offY = -GRID_H; break;
				case 6: l_offX = GRID_W;  l_offY =  GRID_H; break;
				case 7: l_offX = GRID_W;  l_offY = 0;       break;
				case 8: l_offX = GRID_W;  l_offY = -GRID_H; break;
				default: break;
			}
			glBegin(GL_LINES);
			glVertex3f(0.6 + UNIT_SPAN*(x + 0.5),                                  0.35 + UNIT_SPAN*(y + 0.5),                                  0.0);
			glVertex3f(0.6 + UNIT_SPAN*(link->n_atmn->get_xy()[0] + l_offX + 0.5), 0.35 + UNIT_SPAN*(link->n_atmn->get_xy()[1] + l_offY + 0.5), 0.0);
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

	// set up colors 
	// TODO: move all of this back into rgb.h/cpp
	// make sure you use POW.
	// and see if you can figure out why the spans are so small
	// the pastels look nice but contrast is better
	int divisor = get_divisor(NUM_PT);
	float min = 0.5;
	float max = 1.0;

	colors = new float*[pow((divisor + 1), 3)];
	for (int i = 0; i < pow((divisor + 1), 3); i++)
		colors[i] = new float[3];

	divide_colors(divisor, min, max, colors);

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