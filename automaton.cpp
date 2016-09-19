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
static const int GRID_W = 200;
static const int GRID_H = 200;
static const int NUM_PT = 7;  // TODO: colorization currently only handles 7 points 

static int**     GRID;
Automaton*       POINTS[NUM_PT];



////////////////////////////////
// FXNS FOR HANDLING AUTOMATA // 
////////////////////////////////

/* AUTOMATON */
Automaton::Automaton(int x, int y, int ID) { m_pos[0] = x; m_pos[1] = y; m_ID = ID; }
Automaton::~Automaton()
{
	Node* iter = m_neighbors;

	// just clean up the dalloc neighbor list
	if           (iter == NULL)         { return; }
	else { while (iter->n_next != NULL) { Node* next = iter->n_next; delete[](iter); iter = next; }}
	delete[](iter);
}

int   Automaton::get_ID()        { return m_ID; }
int*  Automaton::get_xy()	     { return m_pos; }
char  Automaton::get_state()     { return m_state; }
Node* Automaton::get_neighbors() { return m_neighbors; }

void Automaton::add_neighbor(Automaton* new_neighbor)
{
	Node* iter = m_neighbors;

	// go until you find the end, or find that there is already a match
	// if the current spot is not a match, you have found the end, allocate new node
	if           (iter == NULL)                                         { m_neighbors  = new Node(new_neighbor); }
	else { while (iter->n_next != NULL && iter->n_atmn != new_neighbor) { iter         = iter->n_next; }
		   if    (iter->n_atmn != new_neighbor)                         { iter->n_next = new Node(new_neighbor); }}
}



/////////////////////////////////
// FXNS FOR GET AUTOMATA STATE //
/////////////////////////////////

double get_dist(int x, int y, Automaton* point)
{
	return hypot(double(x - point->get_xy()[0]), double(y - point->get_xy()[1]));
}

int        get_grid_w()         { return GRID_W; }
int        get_grid_h()         { return GRID_H; }
int**      get_grid_state()     { return GRID; }
int        get_num_atmn()       { return NUM_PT; }
Automaton* get_arr_atmn(int index) { return POINTS[index]; }

void print_neighbors(Automaton* point)
{
	Node* iter = point->get_neighbors();
	cout << point->get_ID() << ": ";

	// print out list in order
	if           (iter == NULL)         { cout << endl; return; }
	else { while (iter->n_next != NULL) { cout << iter->n_atmn->get_ID() <<  " "; iter = iter->n_next; }
										  cout << iter->n_atmn->get_ID() << endl; }
}

void print_neighbors_all() { for (int i = 0; i < NUM_PT; i++) { print_neighbors(POINTS[i]); } cout << endl; }


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

		POINTS[i] = new Automaton(x, y, i + 1);
		GRID[x][y] = i + 1;
	}
}

// TODO:
// determine whether this can be simplified to an abstracted calculation
// currently performs an "embodied" calculation. see basic geometry ref:
// https://www.algebra.com/algebra/homework/Length-and-distance/Length-and-distance.faq.question.442382.html
bool radiate()
{
	// create grid to track updates
	// (contains 3d dimension which is hash for each point which radiates here during this frame)
	bool D_GRID[GRID_W][GRID_H][NUM_PT];
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		// initialize with all falses (no occupancy)
		for (int k = 0; k < NUM_PT; k++) { D_GRID[i][j][k] = false; }
	}}

	// for each grid space
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		// if the space was previously empty
		// (because of this specification, we won't assign neighbors due to radial overlap disjoint areas) 
		if (GRID[i][j] == 0)
			// check if it is within radius of points
			for (int k = 0; k < NUM_PT; k++)
				if (get_dist(i, j, POINTS[k]) <= frame)
					D_GRID[i][j][k] = true; // if no other point has radiated here in this frame, occupy
	}}

	// update + check if there was any change
	bool nonempty = false;
	for (int i = 0; i < GRID_W; i++) { for (int j = 0; j < GRID_H; j++)
	{
		int num_occupy =  0;
		int pnt_occupy = -1;
		for (int k = 0; k < NUM_PT; k++)
		{
			// determine how many new occupancies in the position, and from what point the occupancies radiates
			// stores only the latest point, unused if num_occupy > 1
			if (D_GRID[i][j][k])
			{
				num_occupy += 1;
				pnt_occupy  = k+1;
				nonempty    = true;
			}
		}

		// update point properties and grid accordingly
		// TODO: write function that takes bool array for point and assigns the trues as neighbors
		if      (num_occupy  > 1) { GRID[i][j] = -1; assign_neighbors(D_GRID[i][j]); }
		else if (num_occupy == 1) { GRID[i][j] = pnt_occupy; }
	}}

	// error checking
	/*
	for (int i = 0; i < GRID_H; i++) { for (int j = 0; j < GRID_W; j++)
	{
		cout << GRID[j][GRID_H-i-1];
	} cout << endl; }
	cout << endl << frame << endl << endl;
	*/

	frame++;
	return nonempty;
}

void link_two(Automaton* point_a, Automaton* point_b)
{
	point_a->add_neighbor(point_b);
	point_b->add_neighbor(point_a);
}

void assign_neighbors(bool* neighbors)
{
	for (int i = 0; i < NUM_PT; i++) { for (int j = i + 1; j < NUM_PT; j++) {
		if (neighbors[i] && neighbors[j])
			link_two(POINTS[i], POINTS[j]);
	}}
}