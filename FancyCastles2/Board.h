#include "Hex.h"

struct AxialCoord
{
	int r;
	int q;
	AxialCoord() : r(0), q(0) { }
	AxialCoord(int r_in, int q_in) :r(r_in), q(q_in) { }
};

typedef std::map<size_t, AxialCoord> TileMap;
typedef std::vector<std::unique_ptr<HexTile> > TileList;

class Board
{
public:

	Board();

	void MakeBoard(int numTilesPerType);

	AxialCoord  GetTileCoord(size_t tileIndex);
	ResourceType GetTileType(size_t tileIndex) const;
	int GetNumTiles() const { return mNumTiles; }

private:
	void CreateTiles(int numTilesPerType);
	void ShuffleTiles(TileList& tilesToShuffle);
	void ConnectTiles(TileList& tilesForBoard);

	int GetNextHexagonalNumber(const int& curNumTiles) const;

	size_t mNumTiles;
	TileMap mBoard;
	TileList mTiles;
};