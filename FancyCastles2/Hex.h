#ifndef METZE_HEX_H
#define METZE_HEX_H

#include <ctime>

#include "Observer.h"

class TileTimer : public Subject
{
public:
	TileTimer();

	void OnTimerStart();
	void OnTimerDone();
	void Tick();
	bool IsBusy() const { return mTimerState.isBusy; }

private:
	struct TimerState
	{
		bool isBusy;
		time_t startTime;

		TimerState() : isBusy(false), startTime(std::clock()) { }
		void SetBusy()
		{
			isBusy = true;
			startTime = std::clock();
		}
	};

	const double mTimeoutSec;
	TimerState mTimerState;

};

enum ResourceType { WHEAT, ORE, TREE, GRASS, WATER, NUMTYPES };

class HexTile
{

public:
	HexTile() : mHarvestRate(0), mTileType(WATER), mTileOwnerID(-1){ }
	HexTile(ResourceType type, int tileID);

	ResourceType GetTileType() const { return mTileType; }
	
	int GetTileOwnerID() const { return mTileOwnerID; }
	void SetTileOwner(unsigned int playerID) { mTileOwnerID = playerID; }

	int GetHarvestRate() const { return mHarvestRate; }
	int SetHarvestRate(const int& newRate) { mHarvestRate = newRate; }

	void PrintTileInfo() const;

private:

	int mHarvestRate;
	ResourceType mTileType;
	int mTileOwnerID;
	int mTileID;
};


#endif