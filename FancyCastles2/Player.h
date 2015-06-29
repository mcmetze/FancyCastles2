#ifndef METZE_PLAYER_H
#define METZE_PLAYER_H

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Observer.h"

class TileTimer;

class Player : public Observer
{
public:
	Player(const int& id);
	~Player();

	void PrintInfo() const;

	void Tick();

	virtual void OnNotify();

	void AddTile(const int& tileIndex);
	bool SetTimerLocation(const int& tileIndex, const int& harvestRate);

	void StartHarvest();
	void OnHarvestFinish();

private:
	int mPlayerID;
	int mNumBills;
	bool mIsHarvesting;

	int mTimerLocation;
	int mHarvestRateAtTimer;
	std::unique_ptr<TileTimer> mTileTimer;

	std::unordered_map<int, int> mResourcesCount; //tile id -> count

	std::unordered_set<int> mTilesOwned;
	
};

#endif