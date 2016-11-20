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

	cout << divisor << endl;
	cout << n_color << endl;
	for (int i = 0; i < n_color; i++)
	{
		float running_value = i;
		float a = min + span*float(int(running_value) % (divisor + 1));

		running_value = floor(running_value / (divisor + 1));
		float b = min + span*float(int(running_value) % (divisor + 1));

		running_value = floor(running_value / (divisor + 1));
		float c = min + span*float(int(running_value) % (divisor + 1));

		cout << a << endl;
		cout << b << endl;
		cout << c << endl;

		ret[i][0] = a;
		ret[i][1] = b;
		ret[i][2] = c;
	}
}