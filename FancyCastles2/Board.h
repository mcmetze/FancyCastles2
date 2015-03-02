#ifndef METZE_BOARD_H
#define METZE_BOARD_H

#include <vector>
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

struct AxialHash
{
	std::size_t operator () (const AxialCoord& other) const
	{
		// Modified Bernstein hash
		// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
		return (33 * other.r) ^ other.q;
	}
};

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

	void MakeBoard(int numTilesPerType);

	AxialCoord  GetTileCoord(size_t tileIndex);
	ResourceType GetTileType(size_t tileIndex) const;
	bool IsPositionValid(const AxialCoord& position) const;
	int GetNumTiles() const { return mNumTiles; }

	void SetTileOwner(int tileID, int playerID);

	void PrintTileInfo(const int& tileID) const;

private:
	typedef std::map<size_t, AxialCoord> TileMap;
	typedef std::vector<std::unique_ptr<HexTile> > TileList;

	void CreateTiles(int numTilesPerType);
	void ShuffleTiles(TileList& tilesToShuffle);
	void ConnectTiles(TileList& tilesForBoard);

	int GetNextHexagonalNumber(const int& curNumTiles);

	size_t mNumTiles;
	int mNumLayers;

	TileMap mBoard;
	TileList mTiles;
};

#endif