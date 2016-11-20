#ifndef RGB_H_
#define RGB_H_

#include <iostream>
#include <cmath>
using namespace std;

// takes the number of points,
// uses the 3rd root -1 to determine how to divide up the cubic space that exists between that range [min, max]
int get_divisor(int num_pts);

// generates the colors and puts them in ret
void divide_colors(int divisor, float min, float max, float** ret);

#endif
