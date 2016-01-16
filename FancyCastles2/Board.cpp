#include <algorithm>
#include <cassert>
#include <ctime>

#include "Board.h"

Board::Board(): mNumTiles(0)
	, mMapRadius(0)
	, mBoard()
{
}

int 
Board::ComputeNextHexagonalNumber(int seed)
{
	//equation from wikipedia on centered hexagonal numbers
	int mapRadius = 2;
	int hexagonalNumber = 7;
	while (seed > hexagonalNumber)
	{
		++mapRadius;
		hexagonalNumber = (3 * mapRadius)*(mapRadius - 1) + 1;
	}

	mMapRadius = mapRadius - 1;
	return hexagonalNumber;
}

void 
Board::MakeBoard(int numTilesPerType)
{
	//make one of each type of tile for each player
	//then add water tiles until there is enough to make the board a hex
	const auto curNumTiles = static_cast<int>(ResourceType::NUMTYPES)*numTilesPerType;
	mNumTiles = ComputeNextHexagonalNumber(curNumTiles);
	mTiles.reserve(mNumTiles);

	CreateTiles(numTilesPerType);
	ShuffleTiles();
	ArrangeTiles();
}

void 
Board::ShuffleTiles()
{
	srand(time(nullptr));

	//fisher yates shuffle to randomize the tiles in the tile list
	for (int i = mNumTiles - 1; i >= 0; --i)
	{
		auto swapPos = rand() % (i + 1);
		mTiles[i].swap(mTiles[swapPos]);
	}
}

void 
Board::CreateTiles(int numTilesPerType)
{
	//make a tile for each type for each player and add it to the list of tiles
	const int firstResourceType = static_cast<int>(ResourceType::WHEAT);
	const int lastResourceType = static_cast<int>(ResourceType::WATER);
	for (auto curType = firstResourceType; curType < lastResourceType; ++curType)
	{
		const auto curResource = static_cast<ResourceType>(curType);
		for (int curTile = 0; curTile < numTilesPerType; ++curTile)
		{
			mTiles.emplace_back( std::make_unique<Tile>(curResource, mTiles.size()) );
		}
	}

	//the remaining tiles should be water
	while (mTiles.size() < mNumTiles)
	{
		mTiles.emplace_back( std::make_unique<Tile>(ResourceType::WATER, mTiles.size()) );
	}

	assert(mNumTiles == mTiles.size());
}

void 
Board::ArrangeTiles()
{
	//Create the coordinates for the board to make a hexagonal
	// shape and map them to the index of a tile in the tile list
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

bool 
Board::IsPositionValid(const AxialCoord& position) const
{
	return !(abs(position.q) > mMapRadius ||
			 abs(position.r) > mMapRadius ||
			 abs(position.r + position.q) > mMapRadius);
}

bool 
Board::IsTileValid(int tileID) const
{
	return tileID >= 0 && tileID < mNumTiles;
}

int
Board::GetNumTiles() const
{
	return mNumTiles;
}

AxialCoord 
Board::GetTileCoord(int tileID) const
{
	const auto iter = mBoard.find(tileID);
	assert(iter != mBoard.end());
	return iter->second;
}

ResourceType 
Board::GetTileType(int tileID) const
{
	return mTiles[tileID]->GetTileType();
}

int
Board::GetTileIndex(const AxialCoord& coord) const
{
	const auto iter = mPosMap.find(coord);
	assert(iter != mPosMap.end());
	return iter->second;
}

int
Board::GetHarvestRate(int tileID) const
{
	return mTiles[tileID]->GetHarvestRate();
}

void
Board::SetHarvestRate(int tileID, int newRate)
{
	assert(tileID >= 0 && tileID < mNumTiles);
	return mTiles[tileID]->SetHarvestRate(newRate);
}