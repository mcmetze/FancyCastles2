#include "HexGeometry.h"


# define M_PI           3.14159265358979323846

HexGeometry::HexGeometry()
{
	HexGeometry(1);
}

HexGeometry::HexGeometry(int size): 
	mSize(size) 
{
	mHeight = 2.0*mSize;
	mWidth = sqrt(3.0) / 2.0 * mHeight;
}

void HexGeometry::CreateHex()
{
	for (auto corner = 5; corner >=0; --corner)
	{
		float angle = 2.0 * M_PI / 6.0 * (corner + 0.5);	//0.5 offset makes "pointy top" hex
		float cornerX = mCenter.x + mSize * cos(angle);
		float cornerY = mCenter.y + mSize * sin(angle);
		mVertices.push_back({ cornerX, cornerY });
	}
}

