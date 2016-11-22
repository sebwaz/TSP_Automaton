#include "rgb.h"
using namespace std;

int get_divisor(int num_pts)
{
	return int(ceil(cbrt(num_pts)) - 1);
}

void divide_colors(int divisor, float min, float max, float** ret)
{
	float   span = (max - min) / divisor;
	int     n_rgb = 3;
	int     n_color = int(pow((divisor + 1), 3));

	for (int i = 0; i < n_color; i++)
	{
		float running_value = float(i);
		float a = max - span*float(int(running_value) % (divisor + 1));

		running_value = floor(running_value / (divisor + 1));
		float b = max - span*float(int(running_value) % (divisor + 1));

		running_value = floor(running_value / (divisor + 1));
		float c = max - span*float(int(running_value) % (divisor + 1));

		ret[i][0] = a;
		ret[i][1] = b;
		ret[i][2] = c;
	}
}