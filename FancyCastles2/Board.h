#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Tile.h"

class Board
{
public:

	Board();

	void MakeBoard(int numTilesPerType);

	AxialCoord  GetTileCoord(int tileID) const;
	int GetTileIndex(const AxialCoord& coord) const;
	bool IsPositionValid(const AxialCoord& position) const;
	bool IsTileValid(int tileID) const;

	int GetNumTiles() const;
	ResourceType GetTileType(int tileID) const;
	int GetHarvestRate(int tileID) const;
	void SetHarvestRate(int tileID, int newRate);

private:
	using TileCoordMap = std::unordered_map<int, AxialCoord>;
	using TileIDMap = std::unordered_map<AxialCoord, int >;
	using TileList = std::vector < std::unique_ptr<Tile> > ;

	void CreateTiles(int numTilesPerType);
	void ShuffleTiles();
	void ArrangeTiles();

	int ComputeNextHexagonalNumber(int seed);

	int mNumTiles;
	int mMapRadius;

	TileCoordMap mBoard;
	TileList mTiles;
	TileIDMap mPosMap;
};