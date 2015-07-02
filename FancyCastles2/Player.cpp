#include "Player.h"
#include "Hex.h"

#include <cassert>

Player::Player(const int& id) : mPlayerID(id), mNumBills(5), mTimerLocation(0) 
{
	mTileTimer = std::make_unique<TileTimer>();
	mTileTimer->AddObserver(this);
}

Player::~Player()
{
	mTileTimer->RemoveObserver(this);
}

void 
Player::PrintInfo() const
{
	printf("Player %i owns: ", mPlayerID);
	for (const auto& tileIndex : mTilesOwned)
	{
		printf("%i ", tileIndex);
	}
	printf("\n");
}

void
Player::Tick()
{
	mTileTimer->Tick();
}

void 
Player::TakeTileOwnership(const int& tileIndex)
{
	mTilesOwned.insert(tileIndex);
}

bool
Player::SetTimerLocation(const int& tileIndex, const int& harvestRate) 
{
	if (mTileTimer->IsBusy())
		return false;

	if (mTilesOwned.find(tileIndex) == mTilesOwned.end())
	{
		printf("Player %i does not own tile %i\n", mPlayerID, tileIndex);
		return false;
	}

	mTimerLocation = tileIndex;
	mHarvestRateAtTimer = harvestRate;
	return true;
}

void
Player::StartHarvest()
{
	assert( !mTileTimer->IsBusy() );
	printf("Starting harvest..\n");
	mTileTimer->OnTimerStart();
	mIsHarvesting = true;
}

void
Player::OnHarvestFinish()
{
	mIsHarvesting = false;
	if (mResourcesCount.find(mTimerLocation) == mResourcesCount.end())
	{
		mResourcesCount[mTimerLocation] = 0;
	}
	mResourcesCount[mTimerLocation] += mHarvestRateAtTimer;

	printf("Done Harvest. %i resources for tile %i\n", mResourcesCount[mTimerLocation], mTimerLocation);
}

void
Player::OnNotify()
{
	if (mIsHarvesting)
	{
		OnHarvestFinish();
	}
}