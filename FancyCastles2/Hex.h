#ifndef METZE_HEX_H
#define METZE_HEX_H

#include <vector>
#include <memory>
#include <map>

class Timer;
class Player;
class Building;

enum ResourceType { WHEAT, ORE, TREE, GRASS, WATER, NUMTYPES };

class HexTile
{

public:
	HexTile() : mHarvestRate(0), mTileType(WATER){ }
	HexTile(ResourceType type) : mHarvestRate(1), mTileType(type) { }
	HexTile(const HexTile& other);

	ResourceType GetTileType() const { return mTileType; }

private:

	int mHarvestRate;
	ResourceType mTileType;
	//std::map<ResourceType, int> mRawResources;
	//Timer* timer;
	//Player* tileOwner;
	//std::vector<Building*> buildings;
};

#endif