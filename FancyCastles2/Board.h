#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Tile.h"

class Board
{
public:

	Board();

	void MakeBoard(const int& numTilesPerType);

	AxialCoord  GetTileCoord(const int& tileIndex) const;
	ResourceType GetTileType(const int& tileIndex) const;
	int GetTileIndex(const AxialCoord& coord) const;
	int GetHarvestRate(const int& tileIndex) const;
	int GetNumTiles() const { return mNumTiles; }
	std::unordered_set<int> GetNeighbors(const int& tileIndex) const;
	std::unordered_set<int> FindConnectedTilesWithSameOwner(const int& playerID, const int& source) const;

	bool IsPositionValid(const AxialCoord& position) const;

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