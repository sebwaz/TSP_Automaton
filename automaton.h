#ifndef AUTOMATON_H_
#define AUTOMATON_H_

#include <iostream>
#include <time.h>
#include <glut.h>

//////////////////
// ATMN GLOBALS //
//////////////////

static const int NUM_OGNS = 9;



//////////////
// AUTOMATA // 
//////////////

// forward declaration
class Automaton;
class Node;

// each city in TSP will be of the Automaton class
class Automaton
{
public:
	Automaton(int x, int y, int ID);
	~Automaton();
	int   get_ID();
	int*  get_xy();
	char  get_state();
	Node* get_neighbors(); // retuns the head of neighbor list

	/* FOR HANDLING NEIGHBORS */
	bool add_neighbor(Automaton* neighbor, int n_origin); // dynamically adds node to neighbor list
	                                                         // returns whether addition was successfull


private:
	char	   m_state = 'n';
	int		   m_pos[2];
	int        m_ID;
	Automaton* m_links[2]  = { NULL, NULL };
	Node*      m_neighbors =   NULL;
};

class Node
{
public:
	Node(Automaton* atmn, int origin, double dist)
	{
		n_atmn   = atmn;
		n_origin = origin;
		n_dist   = dist; 
	};

	Automaton* n_atmn;
	Node*      n_next = NULL;
	int        n_origin;
	double     n_dist;
	// n_origin specifies which square from a 9-square extrapolation
	// (centered on base map) the neighbor comes from
	// |8|1|2|
	// |7|0|3|
	// |6|5|4|
	// starts at center, then up and clockwise

};



/////////////////////////////////
// FXNS FOR GET AUTOMATA STATE //
/////////////////////////////////

double     get_dist(int x, int y, Automaton* point);
int        get_grid_w();
int        get_grid_h();
int**      get_grid_state();
int        get_num_atmn();
Automaton* get_arr_atmn(int index);
void       print_neighbors(Automaton* point);
void       print_neighbors_all();




/////////////////////////
// FXNS FOR MANIP GRID //
/////////////////////////

// initializes global grid to all 0s (empty),
// then places TSP cities in grid
void InitTSP();

// performs 1-frame radiation.
// returns bool: true if radiation produced change,
// false if radiationdid nothing
bool radiate();

// takes the pointers to two automaton
// adds them to each other's neighbors list
void neighbor_two(Automaton* point_a, int a_origin, Automaton* point_b, int b_origin);

// takes an array of bools of size == num_pt,
// where true indicates that radiation has collided at given position for each
// adds each occupant to each other's neighbors list
void assign_neighbors(bool neighbors[][NUM_OGNS]);

// called by assign_neighbors() after successfully registering
// two points as each other's neighbors to determine how
// automaton should interact with new neighbor
void handle_new_link(Automaton* point_a, Automaton* point_b);

#endif
