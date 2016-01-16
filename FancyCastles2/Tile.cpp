#include "Tile.h"

Tile::Tile(ResourceType type, int tileID) : mTileType(type), mTileID(tileID), mHarvestRate(1)
{
	if (mTileType == ResourceType::WATER)
		mHarvestRate = 0;
}

ResourceType
Tile::GetTileType() const
{
	return mTileType;
}

int
Tile::GetTileID() const
{
	return mTileID;
}

int
Tile::GetHarvestRate() const
{
	return mHarvestRate;
}

void
Tile::SetHarvestRate(int newRate)
{
	if (newRate <= 0)
		return;

	if (mTileType == ResourceType::WATER)
		return;

	mHarvestRate = newRate;
}