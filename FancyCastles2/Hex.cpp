#include "Hex.h"
#include <stdio.h>
#include <iostream>

HexTile::HexTile(ResourceType type, int tileID) : mTileType(type), mTileID(tileID), mHarvestRate(1)
{
	if (mTileType == WATER)
		mHarvestRate = 0;
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




TileTimer::TileTimer() : mTimerState(), mTimeoutSec(5.0) { }


void TileTimer::OnTimerStart()
{
	mTimerState.SetBusy();
}

void TileTimer::OnTimerDone()
{
	mTimerState.isBusy = false;
	Notify();
}

void TileTimer::Tick()
{
	if (mTimerState.isBusy)
	{
		const auto duration = (std::clock() - mTimerState.startTime) / (double(CLOCKS_PER_SEC));
		if (duration - mTimeoutSec > DBL_EPSILON)
		{
			OnTimerDone();
		}
	}
}