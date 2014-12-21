#include <vector>

class HexGeometry
{
	struct Location
	{
		float x;
		float y;
		Location() : x(0.f), y(0.f) { }
		Location(float x_in, float y_in): x(x_in), y(y_in) { }
	};

public:
	HexGeometry();
	HexGeometry(int size);

	void CreateHex();
	
	float GetWidth() const { return mWidth; }
	float GetHeight() const { return mHeight; }
	std::vector<Location> GetHexVerts() const { return mVertices; }

private:

	std::vector<Location> mVertices;
	Location mCenter;
	int mSize;			//distance from center to vertex
	float mWidth;		//distance from edge to edge horizontally
	float mHeight;		//distance from top vertex to bottom vertex
};
