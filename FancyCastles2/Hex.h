#ifndef METZE_HEX_H
#define METZE_HEX_H

class Timer;
class Player;
class Building;

enum ResourceType { WHEAT, ORE, TREE, GRASS, WATER, NUMTYPES };

class HexTile
{

public:
	HexTile() : mHarvestRate(0), mTileType(WATER), mTileOwnerID(-1){ }
	HexTile(ResourceType type);

	ResourceType GetTileType() const { return mTileType; }
	int GetTileOwnerID() const { return mTileOwnerID; }

	void SetTileOwner(int playerID) { mTileOwnerID = playerID; }

	void PrintTileInfo() const;

private:

	int mHarvestRate;
	ResourceType mTileType;
	int mTileOwnerID;

	//std::map<ResourceType, int> mRawResources;
	//Timer* timer;
	//Player* tileOwner;
	//std::vector<Building*> buildings;
};

#endif