#pragma once

#include <memory>

struct AxialCoord
{
	int r, q;
	AxialCoord() : r(0), q(0) { }
	AxialCoord(int r_in, int q_in) :r(r_in), q(q_in) { }

	bool operator==(const AxialCoord& other) const
	{
		return (other.r == r && other.q == q);
	}

	AxialCoord& operator+=(const AxialCoord& rhs) // compound assignment (does not need to be a member,
	{											  // but often is, to modify the private members)
		r += rhs.r;
		q += rhs.q;
		return *this; // return the result by reference
	}

	// friends defined inside class body are inline and are hidden from non-ADL lookup
	friend AxialCoord operator+(      AxialCoord  lhs,       // passing first arg by value helps optimize chained a+b+c
							    const AxialCoord& rhs)		 // alternatively, both parameters may be const references.
	{
		return lhs += rhs; // reuse compound assignment and return the result by value
	}
};

namespace std {
	template <> struct hash<AxialCoord>
	{
		size_t operator()(const AxialCoord& h) const
		{
			hash<int> int_hash;
			size_t hq = int_hash(h.q);
			size_t hr = int_hash(h.r);
			return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
		}
	};
}

struct Color
{
	float r, g, b;

	Color() : r(0.0), g(0.0), b(0.0) { }
	Color(float r_in, float g_in, float b_in) : r(r_in), g(g_in), b(b_in) { }
};

enum ResourceType
{
	WHEAT,
	ORE,
	TREE,
	GRASS,
	WATER,
	NUMTYPES
};
