#include "Board.h"

#include <algorithm>
#include <cassert>

Board::Board():
	  mNumTiles(0)
	, mMapRadius(0)
	, mBoard()
{
}

int 
Board::ComputeNextHexagonalNumber(const int& curNumTiles)
{
	//equation from wikipedia on centered hexagonal numbers
	int mapRadius = 2;
	int hexagonalNumber = 7;
	while (curNumTiles > hexagonalNumber)
	{
		++mapRadius;
		hexagonalNumber = (3 * mapRadius)*(mapRadius - 1) + 1;
	}

	mMapRadius = mapRadius - 1;
	return hexagonalNumber;
}

void 
Board::MakeBoard(const int& numTilesPerType)
{
	//make one of each type of tile for each player
	//then add water tiles until there is enough to make the board a hex
	const auto curNumTiles = (NUMTYPES)*numTilesPerType;
	mNumTiles = ComputeNextHexagonalNumber(curNumTiles);
	mTiles.reserve(mNumTiles);

	CreateTiles(numTilesPerType);
	ShuffleTiles();
	ConnectTiles();
}

void 
Board::ShuffleTiles()
{
	srand(time(nullptr));

	//fisher yates shuffle to randomize the tiles in the tile list
	for (int i = mNumTiles - 1; i > 0; --i)
	{
		auto swapPos = rand() % (i + 1);
		mTiles[i].swap(mTiles[swapPos]);
	}
}

void 
Board::CreateTiles(const int& numTilesPerType)
{
	//make a tile for each type for each player and add it to the list of tiles
	for (int curType = WHEAT; curType < WATER; ++curType)
	{
		ResourceType curResource = static_cast<ResourceType>(curType);
		for (int curTile = 0; curTile < numTilesPerType; ++curTile)
		{
			mTiles.emplace_back( std::make_unique<HexTile>(curResource, mTiles.size()) );
		}
	}

	//the remaining tiles should be water
	while (mTiles.size() < mNumTiles)
	{
		mTiles.emplace_back( std::make_unique<HexTile>(WATER, mTiles.size()) );
	}

	assert(mNumTiles == mTiles.size());
}

void 
Board::ConnectTiles()
{
	int tileIndex = 0;
	for (int q = -mMapRadius; q <= mMapRadius; ++q)
	{
		const int r1 = std::max(-mMapRadius, -q - mMapRadius);
		const int r2 = std::min(mMapRadius, -q + mMapRadius);
		for (int r = r1; r <= r2; ++r)
		{
			AxialCoord curLocation(r, q);
			mBoard[tileIndex] = curLocation;
			mPosMap[curLocation] = tileIndex++;
		}
	}
}

void 
Board::SetTileOwner(const int& tileID, const int& playerID)
{
	assert(tileID >= 0 && tileID < mNumTiles);
	mTiles[tileID]->SetTileOwner(playerID);
}

bool 
Board::IsPositionValid(const AxialCoord& position) const
{
	return !(abs(position.q) > mMapRadius ||
			 abs(position.r) > mMapRadius ||
			 abs(position.r + position.q) > mMapRadius);
}

AxialCoord 
Board::GetTileCoord(const int& tileIndex)
{
	assert(mBoard.find(tileIndex) != mBoard.end());
	return mBoard[tileIndex];
}

ResourceType 
Board::GetTileType(const int& tileIndex) const
{
	assert(tileIndex >= 0 && tileIndex < mNumTiles);
	return mTiles[tileIndex]->GetTileType();
}

int
Board::GetTileIndex(const AxialCoord& coord)
{
	assert(mPosMap.find(coord) != mPosMap.end());
	return mPosMap[coord];
}

int
Board::GetTileOwner(const int& tileIndex) const
{
	assert(tileIndex >= 0 && tileIndex < mNumTiles);
	return mTiles[tileIndex]->GetTileOwnerID();
}

int
Board::GetHarvestRate(const int& tileIndex) const
{
	assert(tileIndex >= 0 && tileIndex < mNumTiles);
	return mTiles[tileIndex]->GetHarvestRate();
}





void
Board::PrintTileInfo(const int& tileIndex) const
{
	assert(tileIndex >= 0 && tileIndex < mNumTiles);
	mTiles[tileIndex]->PrintTileInfo();
}