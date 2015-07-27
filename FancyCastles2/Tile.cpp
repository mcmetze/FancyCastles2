#include <stdio.h>
#include <iostream>

#include "Tile.h"

HexTile::HexTile(ResourceType type, int tileID) : mTileType(type), mTileID(tileID), mHarvestRate(1)
{
	if (mTileType == WATER)
		mHarvestRate = 0;
}

ResourceType
HexTile::GetTileType() const
{
	return mTileType;
}

int
HexTile::GetTileOwnerID() const
{
	return mTileOwnerID;
}

void
HexTile::SetTileOwner(unsigned int playerID)
{
	mTileOwnerID = playerID;
}

int
HexTile::GetHarvestRate() const
{
	return mHarvestRate;
}

void
HexTile::SetHarvestRate(const int& newRate)
{
	mHarvestRate = newRate;
}

void HexTile::PrintTileInfo() const
{
	printf("-----Tile Info-----\n");
	printf("tileID: %i, ", mTileID);
	printf("owner: %i, ", mTileOwnerID);
	printf("harvest rate: %i, ", mHarvestRate);

	switch (mTileType)
	{
	case WATER:
		printf("water\n");
		break;
	case GRASS:
		printf("grass\n");
		break;
	case WHEAT:
		printf("wheat\n");
		break;
	case ORE:
		printf("ore\n");
		break;
	case TREE:
		printf("tree\n");
		break;
	default:
		break;
	}

	printf("\n");
}