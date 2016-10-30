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

bool Automaton::add_neighbor(Automaton* new_neighbor)
{
	Node* iter = m_neighbors;

	// go until you find the end, or find that there is already a match
	// if the current spot is not a match, you have found the end, allocate new node
	if           (iter == NULL)                                         { m_neighbors  = new Node(new_neighbor); return true; }
	else { while (iter->n_next != NULL && iter->n_atmn != new_neighbor) { iter         = iter->n_next; }
	if           (iter->n_atmn != new_neighbor)                         { iter->n_next = new Node(new_neighbor); return true; }
	else                                                                { return false; }}
}



/////////////////////////////////
// FXNS FOR GET AUTOMATA STATE //
/////////////////////////////////

double get_dist(int x, int y, Automaton* point)
{
	return hypot(double(x - point->get_xy()[0]), double(y - point->get_xy()[1]));
}

// overload of get_dist that allows point source to be offset
// (used primarily to shortcut toroidal transformation of map)
double get_dist(int x, int y, Automaton* point, int px_off, int py_off)
{
	return hypot(double(x - (point->get_xy()[0] + px_off)), double(y - (point->get_xy()[1] + py_off)));
}

int        get_grid_w()            { return GRID_W; }
int        get_grid_h()            { return GRID_H; }
int**      get_grid_state()        { return GRID; }
int        get_num_atmn()          { return NUM_PT; }
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
	// (contains 3rd dimension which is hash for each point which radiates here during this frame)
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
					// note that point k will radiate to this space in this frame
					// (multiple simultaneous occupancies will be checked before updating map)
					D_GRID[i][j][k] = true;

				else if (get_dist(i, j, POINTS[k], -GRID_W, -GRID_H) <= frame ||
						 get_dist(i, j, POINTS[k], -GRID_W,       0) <= frame ||
						 get_dist(i, j, POINTS[k], -GRID_W,  GRID_H) <= frame ||
						 get_dist(i, j, POINTS[k],       0,  GRID_H) <= frame ||
						 get_dist(i, j, POINTS[k],  GRID_W,  GRID_H) <= frame ||
						 get_dist(i, j, POINTS[k],  GRID_W,       0) <= frame ||
						 get_dist(i, j, POINTS[k],  GRID_W, -GRID_H) <= frame ||
						 get_dist(i, j, POINTS[k],       0, -GRID_H) <= frame) /* TOROIDAL RADIATION */
					D_GRID[i][j][k] = true;
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

	frame++;
	return nonempty;
}

void neighbor_two(Automaton* point_a, Automaton* point_b)
{
	if (point_a->add_neighbor(point_b) && point_b->add_neighbor(point_a))
		handle_new_link(point_a, point_b);
}

// TODO:
// Be sure neighbor logging happens in distance order!
// Right now, this is not guaranteed. Two neighbors may be added
// in the same frame, and their order depends only on grid check order.
// This may be problematic for automata's linking behavior.

// Moreover, neighbor_two() is often called when the neighbors are not new
// Is this problematic for linking behavior?
void assign_neighbors(bool* neighbors)
{
	for (int i = 0; i < NUM_PT; i++) { for (int j = i + 1; j < NUM_PT; j++) {
		if (neighbors[i] && neighbors[j])
			neighbor_two(POINTS[i], POINTS[j]);
	}}
}

void handle_new_link(Automaton* point_a, Automaton* point_b)
{
	// if this is a joint (links == 2), do nothing
	// if this is an end  (links < 2),  and other is an end  (links < 2)   automatically link
	// if this is an end  (links < 2),  but the other is not (links == 2), special case:
	// check if joint has link which neighbors an end, or an excited
	// if link neighbors end,     break link and connect link to end, connect joint (now end) to this end
	// if link neighbors excited,
	// break excited neighbor from its old link   (which neighbors an end), connect it's old link to its neighbor end
	// break this link and connect it to the excited neighbor (now an end)
	// connect this joint (now an end) to other end
	// if link not neighbor end or excited, change link state to excited (?)

	// should every point have a neighboring-an-end status, that gets updated when neighbor becomes end or becomes joint? 
}