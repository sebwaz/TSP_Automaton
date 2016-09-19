#ifndef AUTOMATON_H_
#define AUTOMATON_H_

#include <iostream>
#include <time.h>
#include <glut.h>



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
	bool add_neighbor(Automaton* neighbor); // dynamically adds node to neighbor list
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
	Node(Automaton* atmn) { n_atmn = atmn; };

	Automaton* n_atmn;
	Node*      n_next = NULL;
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
void neighbor_two(Automaton* point_a, Automaton* point_b);

// takes an array of bools of size == num_pt,
// where true indicates that radiation has collided at given position for each
// adds each occupant to each other's neighbors list
void assign_neighbors(bool* neighbors);

// called by assign_neighbors() after successfully registering
// two points as each other's neighbors to determine how
// automaton should interact with new neighbor
void handle_new_link(Automaton* point_a, Automaton* point_b);

#endif
