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
static const int GRID_W   = 200;
static const int GRID_H   = 200;
static const int NUM_PT   = 7;  // TODO: colorization currently only handles 7 points 

static int**     GRID;
Automaton*       POINTS[NUM_PT];



////////////////////////////////
// FXNS FOR HANDLING AUTOMATA // 
////////////////////////////////

// forward declarations
double get_dist(int x, int y, Automaton* point);
double get_dist(int x, int y, Automaton* point, int px_off, int py_off);

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

// TODO: add a distance value to the neighbor nodes, so that the neighbors can be sorted
// be sure that you don't add two neighbors for the same point and origin if they just have different distances.
// ^ in that case, keep the nearest collision distance (ideally, this would be the first one input,
//   but I can't guarantee since updates are done in x,y order)
bool Automaton::add_neighbor(Automaton* new_neighbor, int n_origin, int coll_x, int coll_y)
{
	Node* iter = this->m_neighbors;
	Node* tail = NULL;
	// get distance to collision
	double new_dist = hypot(double(coll_x - this->get_xy()[0]), double(coll_y - this->get_xy()[1]));

	// check if empty
	if (iter == NULL)
	{ 
		this->m_neighbors  = new Node(new_neighbor, n_origin, new_dist);
		return true;
	}
	else
	{
		// iterate through list 
		// breaking on a match on neighbor point # and origin #
		// OR breaking on last item in list
		while (iter->n_next != NULL && !(iter->n_atmn == new_neighbor && iter->n_origin == n_origin))
			iter = iter->n_next;

		// if you broke at the end, add a new neighbor in SORTED position
		if (!(iter->n_atmn == new_neighbor && iter->n_origin == n_origin))
		{
			iter = this->m_neighbors;
			// iterate through list, while current dist is smaller than new neighbor dist
			while (iter->n_dist <= new_dist)
			{
				// if not at the end, move on
				if (iter->n_next != NULL)
				{
					tail = iter;
					iter = iter->n_next;
				}
				// if at end, add the new neighbor at the end
				else
				{
					iter->n_next = new Node(new_neighbor, n_origin, new_dist);
					return true;
				}
			}
			// if here, we broke out before end
			// .: current is first instance of neighbor with dist >= new
			// insert in list before current
			Node* to_add = new Node(new_neighbor, n_origin, new_dist);
			to_add->n_next = iter;

			// if the break happened after 1st, then link preceding to added
			// else, added is first, so make sure m_neighbors points to it instead
			if (tail != NULL) { tail->n_next      = to_add; }
			else              { this->m_neighbors = to_add; }
			return true;
		}

		// else you broke on a match, so take the one with the shortest distance
		
		else
		{
			// if distance of match greater than distance of current, replace with current
			// else, do nothing
			if (iter->n_dist >= new_dist)
			{
				// remove the old --- delete[](iter)
				// then iterate back through to find the point where the new should be inserted
				// (the neighbors list should always be sorted in terms of distance)
				// by radiation technique, this should be given
				// but can't be guaranteed due to update within frame in x,y order
			}
			// return false because no new point added, just distance updated
			return false;
		}
	}	
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
	else { while (iter->n_next != NULL) { cout << iter->n_atmn->get_ID() << "_" << iter->n_origin <<  " "; iter = iter->n_next; }
										  cout << iter->n_atmn->get_ID() << "_" << iter->n_origin << endl; }
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


bool radiate()
{
	// create grid to track updates
	// (contains 3rd dimension which is hash for each point which radiates here during this frame)
	// (contains 4th dimension which is hash for origin of each radiation)
	bool D_GRID[GRID_W][GRID_H][NUM_PT][NUM_OGNS];
	for (int i = 0; i < GRID_W; i++)
		for (int j = 0; j < GRID_H; j++)
			for (int k = 0; k < NUM_PT; k++)
				for (int m = 0; m < NUM_OGNS; m++)
					// initialize with all falses (no occupancies yet)
					D_GRID[i][j][k][m] = false;

	// for each grid space
	for (int i = 0; i < GRID_W; i++) {
		for (int j = 0; j < GRID_H; j++) {

			// if the space was previously empty
			// (because of this specification, we won't assign neighbors due to radial overlap disjoint areas) 
			if (GRID[i][j] == 0)

				// check if it is within radius of points
				for (int k = 0; k < NUM_PT; k++)
					if (get_dist(i, j, POINTS[k]) <= frame) 
						// note that point k will radiate to this space in this frame
						// (multiple simultaneous occupancies will be checked before updating map)
						D_GRID[i][j][k][4] = true;

					/* FOR TOROIDAL RADIATION: */
					else if (get_dist(i, j, POINTS[k], -GRID_W, -GRID_H) <= frame) { D_GRID[i][j][k][2] = true; }
					else if (get_dist(i, j, POINTS[k], -GRID_W,       0) <= frame) { D_GRID[i][j][k][1] = true; }
					else if (get_dist(i, j, POINTS[k], -GRID_W,  GRID_H) <= frame) { D_GRID[i][j][k][0] = true; }
					else if (get_dist(i, j, POINTS[k],       0,  GRID_H) <= frame) { D_GRID[i][j][k][3] = true; }
					else if (get_dist(i, j, POINTS[k],  GRID_W,  GRID_H) <= frame) { D_GRID[i][j][k][6] = true; }
					else if (get_dist(i, j, POINTS[k],  GRID_W,       0) <= frame) { D_GRID[i][j][k][7] = true; }
					else if (get_dist(i, j, POINTS[k],  GRID_W, -GRID_H) <= frame) { D_GRID[i][j][k][8] = true; }
					else if (get_dist(i, j, POINTS[k],       0, -GRID_H) <= frame) { D_GRID[i][j][k][5] = true; }				
		}
	}

	// iterate through each x,y position
	// update + check if there was any change
	bool nonempty = false;
	for (int i = 0; i < GRID_W; i++) {
		for (int j = 0; j < GRID_H; j++) {

			int num_occupy =  0;
			int pnt_occupy = -1;

			for (int k = 0; k < NUM_PT; k++) {
				for (int m = 0; m < NUM_OGNS; m++) {
				// determine how many new occupancies in the position, and from what point the occupancies radiate
				// pnt_pccupy stores only the latest point (which is useful if num_occupy == 1). unused if num_occupy > 1
					if (D_GRID[i][j][k][m])
					{
						num_occupy += 1;
						pnt_occupy  = k+1;
						nonempty    = true;
					}
				}
			}

		// update point properties and grid accordingly
		// TODO: pass the x,y of the collision to assign_neighbors
		// this way, dist to collision can be used to sort neighborizations
		if      (num_occupy  > 1) { GRID[i][j] = -1; assign_neighbors(D_GRID[i][j], i, j); }
		else if (num_occupy == 1) { GRID[i][j] = pnt_occupy; }
		}
	}

	frame++;
	return nonempty;
}

void neighbor_two(Automaton* point_a, int a_origin, Automaton* point_b, int b_origin, int coll_x, int coll_y)
{
	int a_abs_ogn = a_origin;
	int b_abs_ogn = b_origin;

	int a_offX;
	int a_offY;
	switch (b_abs_ogn)
	{
		case 0: a_origin +=  4; a_offX =  GRID_W; a_offY = -GRID_H; break;
		case 1: a_origin +=  3; a_offX =  GRID_W; a_offY = 0;       break;
		case 2: a_origin +=  2; a_offX =  GRID_W; a_offY =  GRID_H; break;
		case 3: a_origin +=  1; a_offX = 0;       a_offY = -GRID_H; break;
		case 4: a_origin +=  0; a_offX = 0;       a_offY = 0;       break;
		case 5: a_origin += -1; a_offX = 0;       a_offY =  GRID_H; break;
		case 6: a_origin += -2; a_offX = -GRID_W; a_offY = -GRID_H; break;
		case 7: a_origin += -3; a_offX = -GRID_W; a_offY = 0;       break;
		case 8: a_origin += -4; a_offX = -GRID_W; a_offY =  GRID_H; break;
		default: break;
	}

	int b_offX;
	int b_offY;
	switch (a_abs_ogn)
	{
		case 0: b_origin +=  4; b_offX =  GRID_W; b_offY = -GRID_H; break;
		case 1: b_origin +=  3; b_offX =  GRID_W; b_offY = 0;       break;
		case 2: b_origin +=  2; b_offX =  GRID_W; b_offY =  GRID_H; break;
		case 3: b_origin +=  1; b_offX = 0;       b_offY = -GRID_H; break;
		case 4: b_origin +=  0; b_offX = 0;       b_offY = 0;       break;
		case 5: b_origin += -1; b_offX = 0;       b_offY =  GRID_H; break;
		case 6: b_origin += -2; b_offX = -GRID_W; b_offY = -GRID_H; break;
		case 7: b_origin += -3; b_offX = -GRID_W; b_offY = 0;       break;
		case 8: b_origin += -4; b_offX = -GRID_W; b_offY =  GRID_H; break;
		default: break;
	}
	// add_neighbor() assumes calling atmn is at origin
	// so we need to translate a/b relation (and thus coll_x, coll_y) to call add_neighbor() with 4-centered origins
	if (point_a->add_neighbor(point_b, b_origin, coll_x-b_offX, coll_y-b_offY) && point_b->add_neighbor(point_a, a_origin, coll_x-a_offX, coll_y-a_offY))
		handle_new_link(point_a, point_b);
}

// TODO:
// Be sure neighbor logging happens in distance order!
// HOW TO FIX: figure out all neighbors to be added in a given frame, 
// and the distance to each neighbor's collision point.
// Then log neighbors in order of distance

// Right now, distance order is not guaranteed. Two neighbors may be added
// in the same frame, and their order depends only on grid check order.
// This may be problematic for automata's linking behavior.

// Moreover, neighbor_two() is often called when the neighbors are not new
// Is this problematic for linking behavior?
void assign_neighbors(bool neighbors[][NUM_OGNS], int coll_x, int coll_y)
{
	// iterates through every pair of points (no identity pairings)
	for (int i = 0; i < NUM_PT; i++) { for (int j = i + 1; j < NUM_PT; j++) {
		// and assigns as neighbors any origin combos within those pairings
		for (int k = 0; k < NUM_OGNS; k++) { for (int m = 0; m < NUM_OGNS; m++) {
				if (neighbors[i][k] && neighbors[j][m])
					// TODO: given the way these are represented in D_GRID, design new fxn def for handling more useful args
					neighbor_two(POINTS[i], k, POINTS[j], m, coll_x, coll_y);
		}}
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