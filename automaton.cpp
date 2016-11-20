#include "automaton.h"
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <math.h>
using namespace std;



/////////////
// GLOBALS //
/////////////

/* FRAME COUNTER */
static int  frame      = 1;
static bool links_done = false;

/* TSP SPEC */
static int**     GRID;
Automaton*       POINTS[NUM_PT];



////////////////////////////////
// FXNS FOR HANDLING AUTOMATA // 
////////////////////////////////

// forward declarations
double get_dist(int x, int y, Automaton* point);
double get_dist(int x, int y, Automaton* point, int px_off, int py_off);

/* AUTOMATON */
Automaton::Automaton(int x, int y, int ID) { m_pos[0] = x; m_pos[1] = y; m_ID = ID; } // NOTE: ID is always i+1 of POINTS index i
Automaton::~Automaton()
{
	Node* iter = m_neighbors;

	// just clean up the dalloc neighbor list
	if           (iter == NULL)         { return; }
	else { while (iter->n_next != NULL) { Node* next = iter->n_next; delete[](iter); iter = next; }}
	delete[](iter);
}

int    Automaton::get_ID()        { return m_ID; }
int*   Automaton::get_xy()	      { return m_pos; }
char   Automaton::get_state()     { return m_state; }
Node*  Automaton::get_neighbors() { return m_neighbors; }
Node** Automaton::get_links()     { return m_links; }
int    Automaton::get_num_links() { if		(m_links[0] == NULL) { return 0; } 
									else if (m_links[1] == NULL) { return 1; }
									else						 { return 2; }}

bool Automaton::add_neighbor(Automaton* new_neighbor, int n_origin, double c_dist)
{
	Node* iter = this->m_neighbors;
	Node* tail = NULL;
	// get distance to collision
	double new_dist = c_dist;

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
		{
			tail = iter;
			iter = iter->n_next;
		}

		// if you broke at the end, add a new neighbor in SORTED position
		if (!(iter->n_atmn == new_neighbor && iter->n_origin == n_origin))
		{
			iter = this->m_neighbors;
			tail = NULL;
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
		// TODO: verify this works because you fixed it drunk
		else
		{
			// if distance of match greater than distance of current, extract old, change values to current, reinsert
			// else, do nothing
			if (iter->n_dist > new_dist)
			{
				// if match is not the first member
				if (tail != NULL)
				{
					// extract the match
					Node* reclaimed;
					tail->n_next      = iter->n_next;
					reclaimed         = iter;
					reclaimed->n_dist = new_dist;
					reclaimed->n_next = NULL;

					// then iterate back through to find the point where the new should be inserted
					// TODO: following code taken directly from if-case above. optimize to reduce redundant code
					iter = this->m_neighbors;
					tail = NULL;
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
							iter->n_next = reclaimed;
							// return false because no new point added, just distance updated
							return false;
						}
					}
					// if here, we broke out before end
					// .: current is first instance of neighbor with dist >= new
					// insert in list before current
					reclaimed->n_next = iter;

					// if the break happened after 1st, then link preceding to added
					// else, added is first, so make sure m_neighbors points to it instead
					if (tail != NULL) { tail->n_next      = reclaimed; }
					else              { this->m_neighbors = reclaimed; }
					return false;
				}
				// else match is the first member
				else
				{
					iter->n_dist = new_dist;
					return false;
				}
			}
			// return false because no new point added, just distance updated
			return false;
		}
	}	
}

bool Automaton::add_link(Node* link)
{
	for (int i = 0; i < 2; i++)
	{
		// if link is already linked, do nothing, return false because nothing added
		if (m_links[i] == link) { return false; }

		// else if there is an empty slot and there is no duplicate, add the link, and return true
		else if (m_links[i] == NULL)
		{
			m_links[i] = link;
			cout << this->get_ID() << " linked to " << link->n_atmn->get_ID() << endl;
			return true;
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
	else { while (iter->n_next != NULL) { cout << iter->n_atmn->get_ID() << "-o" << iter->n_origin << " "; iter = iter->n_next; }
										  cout << iter->n_atmn->get_ID() << "-o" << iter->n_origin << endl; }
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
		// NOTE: ID is always i+1 of POINTS index i
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
						pnt_occupy  = k+1; //label for point is i+1 of its POINTS index i
						nonempty    = true;
					}
				}
			}

		// update point properties and grid accordingly
		// this way, dist to collision can be used to sort neighborizations
		if      (num_occupy  > 1) { GRID[i][j] = -1; assign_neighbors(D_GRID[i][j], i, j); }
		else if (num_occupy == 1) { GRID[i][j] = pnt_occupy; }
		}
	}

	// TODO: call at very end so that repeated calls don't interfere with one another?
	// TODO: but if you don't update frame by frame, figure out how to draw in real-time for best demo
	// make sure its IDEMPOTENT!
	// assign_links(); // moved to main.cpp
	frame++;
	return nonempty;
}

void neighbor_two(Automaton* point_a, int a_origin, Automaton* point_b, int b_origin, int coll_x, int coll_y)
{
	int a_abs_ogn = a_origin;
	int b_abs_ogn = b_origin;

	int b_offX;
	int b_offY;
	switch (b_abs_ogn)
	{
		case 0: a_origin +=  4; b_offX = -GRID_W; b_offY =  GRID_H; break;
		case 1: a_origin +=  3; b_offX = -GRID_W; b_offY = 0;       break;
		case 2: a_origin +=  2; b_offX = -GRID_W; b_offY = -GRID_H; break;
		case 3: a_origin +=  1; b_offX = 0;       b_offY =  GRID_H; break;
		case 4: a_origin +=  0; b_offX = 0;       b_offY = 0;       break;
		case 5: a_origin += -1; b_offX = 0;       b_offY = -GRID_H; break;
		case 6: a_origin += -2; b_offX =  GRID_W; b_offY =  GRID_H; break;
		case 7: a_origin += -3; b_offX =  GRID_W; b_offY = 0;       break;
		case 8: a_origin += -4; b_offX =  GRID_W; b_offY = -GRID_H; break;
		default: break;
	}

	int a_offX;
	int a_offY;
	switch (a_abs_ogn)
	{
		case 0: b_origin +=  4; a_offX = -GRID_W; a_offY =  GRID_H; break;
		case 1: b_origin +=  3; a_offX = -GRID_W; a_offY = 0;       break;
		case 2: b_origin +=  2; a_offX = -GRID_W; a_offY = -GRID_H; break;
		case 3: b_origin +=  1; a_offX = 0;       a_offY =  GRID_H; break;
		case 4: b_origin +=  0; a_offX = 0;       a_offY = 0;       break;
		case 5: b_origin += -1; a_offX = 0;       a_offY = -GRID_H; break;
		case 6: b_origin += -2; a_offX = GRID_W;  a_offY =  GRID_H; break;
		case 7: b_origin += -3; a_offX = GRID_W;  a_offY = 0;       break;
		case 8: b_origin += -4; a_offX = GRID_W;  a_offY = -GRID_H; break;
		default: break;
	}

	// add_neighbor() assumes calling atmn is at origin
	// so we need to translate a/b relation (and thus coll_x, coll_y) to call add_neighbor() with 4-centered origins
	// we also use the shortest collision distance calculated between the two, because disparity true value is due to discreteness of map
	// earliest point of collision is equidistant from both points, so c_dist at least the min
	double a_dist = hypot(double(coll_x - (point_a->get_xy()[0] + a_offX)), double(coll_y - (point_a->get_xy()[1] + a_offY)));
	double b_dist = hypot(double(coll_x - (point_b->get_xy()[0] + b_offX)), double(coll_y - (point_b->get_xy()[1] + b_offY)));
	double c_dist = (a_dist < b_dist) ? a_dist : b_dist;

	point_a->add_neighbor(point_b, b_origin, c_dist);
	point_b->add_neighbor(point_a, a_origin, c_dist);
}

// TODO: set up linking beahvior so that it can be done in real-time
void link_two(Node* point_a, Node* point_b)
{
	// TODO: remove the couts for error checking
	cout << "Joining: " << point_a->n_atmn->get_ID() << " and " << point_b->n_atmn->get_ID() << endl;

	// if both are a joint (links == 2), do nothing
	if (point_a->n_atmn->get_num_links() == 2 && point_b->n_atmn->get_num_links() == 2)
	{
		cout << "Both are joints: do nothing." << endl;
		return;
	}

	// if one is an end  (links < 2),  and other is an end  (links < 2)   automatically link
	if (point_a->n_atmn->get_num_links() < 2 && point_b->n_atmn->get_num_links() < 2)
	{
		cout << "Both are ends: join." << endl;
		point_a->n_atmn->add_link(point_b);
		point_b->n_atmn->add_link(point_a);
		return;
	}
	
	// if one is an end  (links < 2),  but the other is not (links == 2), special case:
	if ((point_a->n_atmn->get_num_links()  < 2 && point_b->n_atmn->get_num_links() == 2) ||
		(point_a->n_atmn->get_num_links() == 2 && point_b->n_atmn->get_num_links()  < 2))
	{
		Automaton* joint = (point_a->n_atmn->get_num_links() == 2) ? point_a->n_atmn : point_b->n_atmn;
		Automaton* end   = (point_a->n_atmn->get_num_links() == 2) ? point_b->n_atmn : point_a->n_atmn;
		cout << joint->get_ID() << " is joint. " << end->get_ID() << " is end." << endl;

		int long_l_pos  = (joint->get_links()[0]->n_dist > joint->get_links()[1]->n_dist) ? 0 : 1;
		int short_l_pos = (joint->get_links()[0]->n_dist > joint->get_links()[1]->n_dist) ? 1 : 0;
		Automaton* joint_long_l  = joint->get_links()[long_l_pos]->n_atmn;
		Automaton* joint_short_l = joint->get_links()[short_l_pos]->n_atmn;
		
		Node* long_l_neighbs =  joint_long_l->get_neighbors();
		Node* short_l_neighbs = joint_short_l->get_neighbors();

		// check if joint has link which neighbors an end
		// how to choose if both links neighbor ends?       more distant link first
		// how to choose if a link neighbors multiple ends? nearest neighbor first

		////////////////
		// check long //
		////////////////
		while (long_l_neighbs != NULL)
		{
			// we want to be sure that long_l_neighbs is not already a link of joint_long_l
			bool is_new_link = true;
			for (int i = 0; i < 2; i++)
				if (joint_long_l->get_links()[i] != NULL)
					if (joint_long_l->get_links()[i] == long_l_neighbs)
						is_new_link = false;

			// we want to be sure that, if long_l_neighbs is the end, it is 0-linked
			bool end_check;
			if (long_l_neighbs->n_atmn == end)
			{
				if (long_l_neighbs->n_atmn->get_num_links() == 0) { end_check = true; }
				else                                              { end_check = false; }
			}
			else
			{
				end_check = true;
			}

			// joint_long_l's neighbor should be at a distance less than distance between joint and end
			// this is because we don't currently call assign_links multiple times, (i.e. between frames)
			// so we only want neighbors that would be discovered at the time link is being attempted between joint and end

			// if joint_long_l has a neighbor that is an end, and is within the same COLLISION distance as the distance between joint and end
			// point_a is chosen for getting the joint-to-end collision distance arbitrarily; using point_b should give same value
			if (long_l_neighbs->n_atmn->get_num_links() < 2 && long_l_neighbs->n_dist <= point_a->n_dist && is_new_link && end_check)
			{
				// add joint_long_l as a link to that neighbor
				Node* l_nearest_neighb_l = long_l_neighbs->n_atmn->get_neighbors();
				while (l_nearest_neighb_l != NULL)
				{
					if (l_nearest_neighb_l->n_atmn   == joint_long_l &&
						l_nearest_neighb_l->n_origin == abs(long_l_neighbs->n_origin - 8))
					{
						long_l_neighbs->n_atmn->add_link(l_nearest_neighb_l);
						break;
					}
					l_nearest_neighb_l = l_nearest_neighb_l->n_next;
				}

				// link joint_long_l to the neighbor that just linked to it, by overwriting its link to joint
				for (int i = 0; i < 2; i++)
					if (joint_long_l->get_links()[i]->n_atmn   == joint &&
						joint_long_l->get_links()[i]->n_origin == abs(joint->get_links()[long_l_pos]->n_origin - 8))
					{
						joint_long_l->get_links()[i] = long_l_neighbs;
						cout << joint_long_l->get_ID() << " linked to " << joint_long_l->get_links()[i]->n_atmn->get_ID() << endl;
						break;
					}
				
				// link joint to the end, and end to joint
				// TODO: this originally produced bad links if end was the neighbor of joint_long_l and already had 1 link
				// TODO: if necessary (this works atm), set it up so that joint's linking is more considerate of changes in its neighbors 
				joint->get_links()[long_l_pos] = (point_a->n_atmn == end) ? point_a : point_b;
				cout << joint->get_ID() << " linked to " << joint->get_links()[long_l_pos]->n_atmn->get_ID() << endl;

				end->add_link((point_a->n_atmn == joint) ? point_a : point_b);
				cout << end->get_ID() << " linked to " << ((point_a->n_atmn == joint) ? point_a->n_atmn->get_ID() : point_b->n_atmn->get_ID()) << endl;
				return;
			}
			long_l_neighbs = long_l_neighbs->n_next;
		}

		// if no match through long link:

		/////////////////
		// check short //
		/////////////////
		while (short_l_neighbs != NULL)
		{
			// we want to be sure that short_l_neighbs is not already a link of joint_short_l
			bool is_new_link = true;
			for (int i = 0; i < 2; i++)
				if (joint_short_l->get_links()[i] != NULL)
					if (joint_short_l->get_links()[i] == short_l_neighbs)
						is_new_link = false;

			// we want to be sure that, if short_l_neighbs is the end, it is 0-linked
			bool end_check;
			if (short_l_neighbs->n_atmn == end)
			{
				if (short_l_neighbs->n_atmn->get_num_links() == 0) { end_check = true; }
				else { end_check = false; }
			}
			else
			{
				end_check = true;
			}

			// joint_short_l's neighbor should be at a distance less than distance between joint and end
			// this is because we don't currently call assign_links multiple times, (i.e. between frames)
			// so we only want neighbors that would be discovered at the time link is being attempted between joint and end

			// if joint_short_l has a neighbor that is an end, and is within the same COLLISION distance as the distance between joint and end
			// point_a is chosen for getting the joint-to-end collision distance arbitrarily; using point_b should give same value
			if (short_l_neighbs->n_atmn->get_num_links() < 2 && short_l_neighbs->n_dist <= point_a->n_dist && is_new_link && end_check)
			{
				// add joint_short_l as a link to that neighbor
				Node* s_nearest_neighb_l = short_l_neighbs->n_atmn->get_neighbors();
				while (s_nearest_neighb_l != NULL)
				{
					if (s_nearest_neighb_l->n_atmn == joint_short_l &&
						s_nearest_neighb_l->n_origin == abs(short_l_neighbs->n_origin - 8))
					{
						short_l_neighbs->n_atmn->add_link(s_nearest_neighb_l);
						break;
					}
					s_nearest_neighb_l = s_nearest_neighb_l->n_next;
				}

				// link joint_short_l to the neighbor that just linked to it, by overwriting its link to joint
				for (int i = 0; i < 2; i++)
					if (joint_short_l->get_links()[i]->n_atmn == joint &&
						joint_short_l->get_links()[i]->n_origin == abs(joint->get_links()[short_l_pos]->n_origin - 8))
					{
						joint_short_l->get_links()[i] = short_l_neighbs;
						cout << joint_short_l->get_ID() << " linked to " << joint_short_l->get_links()[i]->n_atmn->get_ID() << endl;
						break;
					}

				// link joint to the end, and end to joint
				// TODO: this originally produced bad links if end was the neighbor of joint_short_l and already had 1 link
				// TODO: if necessary (this works atm), set it up so that joint's linking is more considerate of changes in its neighbors 
				joint->get_links()[short_l_pos] = (point_a->n_atmn == end) ? point_a : point_b;
				cout << joint->get_ID() << " linked to " << joint->get_links()[short_l_pos]->n_atmn->get_ID() << endl;

				end->add_link((point_a->n_atmn == joint) ? point_a : point_b);
				cout << end->get_ID() << " linked to " << ((point_a->n_atmn == joint) ? point_a->n_atmn->get_ID() : point_b->n_atmn->get_ID()) << endl;
				return;
			}
			short_l_neighbs = short_l_neighbs->n_next;
		}
		
		// TODO: excited behavior
		// if link neighbors an excited,
			// break excited neighbor from its old link   (which neighbors an end), connect it's old link to its neighbor end
			// break this link and connect it to the excited neighbor (now an end)
			// connect this joint (now an end) to other end
		
		// if link not neighbor end or excited, change link state to excited (?)
	}
	// should every point have a neighboring-an-end status, that gets updated when neighbor becomes end or becomes joint?
}

// neighbor_two() is often called when the neighbors are not new
// is this problematic for linking behavior?
void assign_neighbors(bool neighbors[][NUM_OGNS], int coll_x, int coll_y)
{
	// iterates through every pair of points (no identity pairings)
	for (int i = 0; i < NUM_PT; i++) { for (int j = i + 1; j < NUM_PT; j++) {
		// and assigns as neighbors any origin combos within those pairings
		for (int k = 0; k < NUM_OGNS; k++) { for (int m = 0; m < NUM_OGNS; m++) {
			if (neighbors[i][k] && neighbors[j][m])
			{ 
				// TODO: given the way these are represented in D_GRID, design new fxn def for handling more useful args
				neighbor_two(POINTS[i], k, POINTS[j], m, coll_x, coll_y);
			}
		}}
	}}
}

// TODO: link handling should only happen between frames and in distance order!
void assign_links()
{
	// only assign_links once
	if (links_done) { return; }
	else { cout << "assign_links() called" << endl; links_done = true; }

	// create a set of pointers to iterate through the list 
	Node** neighbor_iters = new Node*[NUM_PT];
	int neighbor_progs[NUM_PT];
	for (int i = 0; i < NUM_PT; i++)
	{
		neighbor_iters[i] = POINTS[i]->get_neighbors();
		neighbor_progs[i] = 0;
	}

	// iterate through the neighbors in order of collision and conduct linking
	// the distance could never possibly be larger than double the largest dimension of the map.
	double dub_max = 2 * ((GRID_H < GRID_W) ? GRID_W : GRID_H);
	double d_shortest;
	int    n_shortest;
	int    counterpart;

	while (true)
	{
		//it's a safe initial value to compare all others against
		d_shortest = dub_max;
		for (int i = 0; i < NUM_PT; i++)
		{
			if (neighbor_iters[i] != NULL)
			{
				// set d_shortest to whichever is shorter: d_shortest or current n_atmn
				// if two neighbors have equal distance, be sure to use the one whose counterpart has already been reached
				// else you might skip over some.
				// TODO: does this introduce some sort of bias? biased toward last points
				if (neighbor_iters[i]->n_dist <= d_shortest &&
				   (neighbor_iters[(neighbor_iters[i]->n_atmn->get_ID() - 1)]->n_atmn->get_ID() - 1) == i &&
					neighbor_iters[(neighbor_iters[i]->n_atmn->get_ID() - 1)]->n_origin				 == abs(neighbor_iters[i]->n_origin - 8))
				{
					d_shortest  = neighbor_iters[i]->n_dist;
					n_shortest  = i;
					counterpart = (neighbor_iters[i]->n_atmn->get_ID() - 1);
				}
			}
		}

		// d_shortest == dub_max implies all ptrs at NULL (because no n_dist will be > dub_max)
		if (d_shortest == dub_max)
		{
			break;
		}
		else
		{
			// error check:
			cout << "Pair: " << n_shortest+1 << "," << counterpart+1 << endl;

			// attempt to link the two
			link_two(neighbor_iters[n_shortest], neighbor_iters[counterpart]);

			// move n_shortest to next position
			// move n_shortest's counterpart's iterator to position after counterpart neighbor node
			neighbor_iters[counterpart] = neighbor_iters[counterpart]->n_next;
			neighbor_iters[n_shortest]  = neighbor_iters[n_shortest]->n_next;
			neighbor_progs[counterpart]++;
			neighbor_progs[n_shortest]++;

			//error check:
			for (int i = 0; i < NUM_PT; i++)
			{
				if (neighbor_iters[i] != NULL)
					cout << i+1 << "-next: " << neighbor_iters[i]->n_atmn->get_ID() << endl;
				else
					cout << i+1 << "-next: NULL" << endl;
			}
			cout << endl;
		}
	}

	// printout of linkages, for error checking
	for (int i = 0; i < NUM_PT; i++)
	{
		if (POINTS[i]->get_links()[0] == NULL) { cout << i + 1 << ": NULL"; }
		else								   { cout << i + 1 << ": " << POINTS[i]->get_links()[0]->n_atmn->get_ID() << "-o" << POINTS[i]->get_links()[0]->n_origin; }
		if (POINTS[i]->get_links()[1] == NULL) { cout << ", NULL"; }
		else								   { cout <<          ", " << POINTS[i]->get_links()[1]->n_atmn->get_ID() << "-o" << POINTS[i]->get_links()[1]->n_origin << endl; }
	}

	//deallocate the the array of list iterators
	delete[](neighbor_iters);
}