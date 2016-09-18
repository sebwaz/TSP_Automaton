#include "automaton.h"
#include <iostream>
#include <time.h>
#include <math.h>
using namespace std;



/////////////
// GLOBALS //
/////////////

/* FRAME COUNTER */
static int frame = 1;

/* TSP SPEC */
static const int GRID_W = 100;
static const int GRID_H = 100;
static const int NUM_PT = 7;  // TODO: colorization currently only handles 7 points 

static int**     GRID;
Automaton*       POINTS[NUM_PT];



////////////////////////////////
// FXNS FOR HANDLING AUTOMATA // 
////////////////////////////////

Automaton::Automaton(int x, int y)
{
	pos[0] = x;
	pos[1] = y;
}

int* Automaton::get_xy()
{
	return pos;
}



/////////////////////////////////
// FXNS FOR GET AUTOMATA STATE //
/////////////////////////////////

double get_dist(int x, int y, Automaton* point)
{
	return hypot(double(x - point->get_xy()[0]), double(y - point->get_xy()[1]));
}

int    get_grid_w()     { return GRID_W; }
int    get_grid_h()     { return GRID_H; }
int**  get_grid_state() { return GRID; }



/////////////////////////
// FXNS FOR MANIP GRID //
/////////////////////////

void InitTSP()
{
	// allocate grid (global multi-dim array doesn't behave well)
	if (GRID == NULL)
	{
		GRID = new int*[GRID_W];
		for (int i = 0; i < GRID_W; i++)
		{
			GRID[i] = new int[GRID_H];
			for (int j = 0; j < GRID_H; j++) { GRID[i][j] = 0; }
	}	}

	// initialize map with empty values
	for (int i = 0; i < GRID_W; i++)
	{	for (int j = 0; j < GRID_H; j++) { GRID[i][j] = 0; }
	}

	// initialize array containing ptrs to each automata
	srand(time(NULL));
	for (int i = 0; i < NUM_PT; i++)
	{
		// TODO: allow x,y to be specified. optimize rand placement @ high density
		int x;
		int y;
		do
		{
			x = rand() % GRID_W;
			y = rand() % GRID_H;
		} while (GRID[x][y] != 0);

		POINTS[i] = new Automaton(x, y);
		GRID[x][y] = i + 1;
	}
}

bool radiate()
{
	// create grid to track updates
	int D_GRID[GRID_W][GRID_H];
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		// initialize with all zeroes
		D_GRID[i][j] = 0;
	}}

	// for each grid space
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		// if the space was previously empty
		if (GRID[i][j] == 0)
			// check if it is within radius of points
			for (int k = 0; k < NUM_PT; k++)
				if (get_dist(i, j, POINTS[k]) <= frame)
					if (D_GRID[i][j] == 0) { D_GRID[i][j] = k + 1; }      // if no other point has radiated here in this frame, occupy
					else				   { D_GRID[i][j] = NUM_PT + 2; } // if another  point has radiated here in this frame, nullify (put non-zero, non-colorized digit) 
	}}

	// update + check if there was any change
	bool nonzero = false;
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		if (D_GRID[i][j] != 0) { nonzero = true; GRID[i][j] += D_GRID[i][j]; }
	}}

	// error checking
	for (int i = 0; i < GRID_H; i++) { for (int j = 0; j < GRID_W; j++)
	{
			cout << GRID[j][GRID_H-i-1];
	} cout << endl; }
	cout << endl << frame << endl << endl;

	frame++;
	return nonzero;
}