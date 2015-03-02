#include "Hex.h"
#include <stdio.h>

HexTile::HexTile(ResourceType type) : mTileType(type), mHarvestRate(1)
{
	if (mTileType == WATER)
		mHarvestRate = 0;
}

void HexTile::PrintTileInfo() const
{
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

	if (mTileOwnerID >= 0) 
		printf("owner= %i\n", mTileOwnerID);

	printf("harvest rate = %i\n", mHarvestRate);

	printf("\n");
}