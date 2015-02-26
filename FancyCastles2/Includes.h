#include <vector>
#include <memory>
#include <map>

enum ResourceType { WHEAT, ORE, TREE, GRASS, WATER, NUMTYPES };

struct AxialCoord
{
	int r, q;
	AxialCoord() : r(0), q(0) { }
	AxialCoord(int r_in, int q_in) :r(r_in), q(q_in) { }
};

struct Color
{
	int r, g, b;

	Color() : r(0), g(0), b(0) { }
	Color(int r_in, int g_in, int b_in) : r(r_in), g(g_in), b(b_in) { }
};