#pragma once

#include "TileTraits.h"

class Tile
{

public:
	Tile(ResourceType type, int tileID);

	ResourceType GetTileType() const;

	int GetTileID() const;

	int GetHarvestRate() const;
	void SetHarvestRate(int newRate);

private:
	//invariant properties
	ResourceType mTileType;
	int mTileID;
	
	//variant properties
	int mHarvestRate;
};
