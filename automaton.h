#ifndef AUTOMATON_H_
#define AUTOMATON_H_

#include <iostream>
#include <time.h>
#include <glut.h>



//////////////
// AUTOMATA // 
//////////////

// each city in TSP will be of the Automaton class
class Automaton
{
public:
	Automaton(int x, int y);
	~Automaton();
	int* get_xy();

private:
	char state = 'n';
	int  pos[2];
	Automaton*  links[2] = { NULL, NULL };
	Automaton** neighbors;
};



/////////////////////////////////
// FXNS FOR GET AUTOMATA STATE //
/////////////////////////////////

double get_dist(int x, int y, Automaton* point);
int    get_grid_w();
int    get_grid_h();
int**  get_grid_state();



/////////////////////////
// FXNS FOR MANIP GRID //
/////////////////////////

// initializes global grid to all 0s (empty), then places TSP cities in grid
void InitTSP();

// performs 1-frame radiation. returns bool: true if radiation produced change, false if radiationdid nothing
bool radiate();

#endif
