#pragma once

#include "TileTraits.h"

class HexTile
{

public:
	HexTile() : mHarvestRate(0), mTileType(WATER), mTileOwnerID(-1){ }
	HexTile(ResourceType type, int tileID);

	ResourceType GetTileType() const;
	
	int GetTileOwnerID() const;
	void SetTileOwner(unsigned int playerID);

	int GetHarvestRate() const;
	void SetHarvestRate(const int& newRate);

	void PrintTileInfo() const;

private:

	int mHarvestRate;
	ResourceType mTileType;
	int mTileOwnerID;
	int mTileID;
};
