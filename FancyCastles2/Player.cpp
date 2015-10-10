#include <cassert>

#include "Player.h"
#include "Timer.h"

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
	auto ownedTilesIter = mTilesOwned.find(tileIndex);
	if (ownedTilesIter == mTilesOwned.end())
		mTilesOwned.insert(tileIndex);
}

int
Player::GetRawResourcesOnTile(const int& tileIndex) const
{
	const auto& iter = mResourcesCount.find(tileIndex);
	if (iter != mResourcesCount.end())
		return iter->second;

	return 0;
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
	assert(!mTileTimer->IsBusy());

	printf("Starting harvest..\n");
	mTileTimer->OnTimerStart();
	mIsHarvesting = true;
}

void
Player::OnHarvestFinish()
{
	mIsHarvesting = false;
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