#ifndef METZE_BOARD_H
#define METZE_BOARD_H

#include <map>
#include <unordered_map>
#include <memory>

#include "Hex.h"

struct AxialCoord
{
	int r, q;
	AxialCoord() : r(0), q(0) { }
	AxialCoord(int r_in, int q_in) :r(r_in), q(q_in) { }

	bool operator==(const AxialCoord& other) const
	{ 
		return (other.r == r && other.q == q); 
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


class Board
{
public:

	Board();

	void MakeBoard(const int& numTilesPerType);

	AxialCoord  GetTileCoord(const int& tileIndex);
	ResourceType GetTileType(const int& tileIndex) const;
	int GetTileIndex(const AxialCoord& coord);
	int GetHarvestRate(const int& tileIndex) const;

	bool IsPositionValid(const AxialCoord& position) const;
	int GetNumTiles() const { return mNumTiles; }

	void SetTileOwner(const int& tileIndex, const int& playerID);
	int GetTileOwner(const int& tileIndex) const;

	void PrintTileInfo(const int& tileIndex) const;

private:
	typedef std::unordered_map<int, AxialCoord> TileMap;
	typedef std::unordered_map<AxialCoord, int > TileIndexLookup;
	typedef std::vector<std::unique_ptr<HexTile> > TileList;

	void CreateTiles(const int& numTilesPerType);
	void ShuffleTiles();
	void ConnectTiles();

	int ComputeNextHexagonalNumber(const int& curNumTiles);

	int mNumTiles;
	int mMapRadius;

	TileMap mBoard;
	TileList mTiles;
	TileIndexLookup mPosMap;
};

#endif