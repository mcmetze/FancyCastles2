#include "Board.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

Board::Board():
	  mNumTiles(0)
	, mBoard()
{

}

int Board::GetNextHexagonalNumber(const int& curNumTiles) const
{
	//equation from wikipedia on centered hexagonal numbers
	int n = 2;
	int hexagonalNumber = 7;
	while (curNumTiles > hexagonalNumber)
	{
		++n;
		hexagonalNumber = (3 * n)*(n - 1) + 1;
	}

	return hexagonalNumber;
}

void Board::MakeBoard(int numTilesPerType)
{
	//make one of each type of tile for each player
	//then add water tiles until there is enough to make the board a hex
	int curNumTiles = (NUMTYPES)*numTilesPerType;
	mNumTiles = GetNextHexagonalNumber(curNumTiles);
	mTiles.reserve(mNumTiles);

	CreateTiles(numTilesPerType);
	ShuffleTiles(mTiles);
	ConnectTiles(mTiles);
}

void Board::ShuffleTiles(TileList& tilesToShuffle)
{
	srand(time(nullptr));

	//fisher yates shuffle to randomize the tiles in the tile list
	for (auto i = mNumTiles-1; i > 0; --i)
	{
		auto swapPos = rand() % (i + 1);
		tilesToShuffle[i].swap(tilesToShuffle[swapPos]);
	}
}

void Board::CreateTiles(int numTilesPerType)
{
	//make a tile for each type for each player and add it to the list of tiles
	for (int curType = WHEAT; curType < WATER; ++curType)
	{
		ResourceType curResource = static_cast<ResourceType>(curType);
		for (int curTile = 0; curTile < numTilesPerType; ++curTile)
		{
			mTiles.emplace_back(std::make_unique<HexTile>(curResource));
		}
	}

	//the remaining tiles should be water
	while (mTiles.size() < mNumTiles)
	{
		mTiles.emplace_back(std::make_unique<HexTile>(WATER));
	}
}

void Board::ConnectTiles(TileList& tilesForBoard)
{
	int tileIndex = 0;
	int numLayers = (mNumTiles - 1) / 12;
	int startR = 0;
	int startQ = -numLayers;
	
	//compute the axial coordinates for the top half of the board 
	while (startQ <= 0)
	{
		int curR = startR;
		while (curR <= numLayers)
		{
			AxialCoord curLocation(curR++, startQ);
			mBoard[tileIndex++] = curLocation;
		}
		startR--;
		startQ++;
	}

	//compute the axial coordinates for the bottom half of the board
	int endR = numLayers-1;
	while (startQ <= numLayers)
	{
		int curR = -numLayers;
		while (curR <= endR)
		{
			AxialCoord curLocation(curR++, startQ);
			mBoard[tileIndex++] = curLocation;
		}
		endR--;
		startQ++;
	}
}


AxialCoord Board::GetTileCoord(size_t tileIndex)
{
	return mBoard[tileIndex];
}

ResourceType Board::GetTileType(size_t tileIndex) const
{
	return mTiles[tileIndex]->GetTileType();
}