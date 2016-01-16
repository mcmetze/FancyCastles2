#include "Player.h"

#include "GameObject.h"
#include "Timer.h"

Player::Player(int playerID, TimerPtr timer, int numBills)
	: mPlayerID(playerID), mTimer(std::move(timer)), mNumBills(numBills)
{
}

void
Player::AddBills(int numBillsToAdd)
{
	mNumBills += numBillsToAdd;
}

void
Player::TakeBills(int numBillsToTake)
{
	mNumBills -= numBillsToTake;
}

int
Player::GetNumBills() const
{
	return mNumBills;
}

int
Player::GetPlayerID() const
{
	return mPlayerID;
}

void
Player::AddTile(int tileID)
{
	mPlayerTileIDs.insert(tileID);
}

void
Player::RemoveTile(int tileID)
{
	mPlayerTileIDs.erase(tileID);
}

bool
Player::OwnsTile(int tileID) const
{
	return mPlayerTileIDs.count(tileID) > 0;
}

TileIDSet
Player::GetPlayerTileIDs() const
{
	return mPlayerTileIDs;
}

void
Player::AddGameObject(GameObjectPtr obj)
{
	mPlayerObjects.insert(obj);
}

void
Player::RemoveGameObject(GameObjectPtr obj)
{
	mPlayerObjects.erase(obj);
}

GameObjectSet
Player::GetGameObjects() const
{
	return mPlayerObjects;
}

TimerObject&
Player::GetTimer()
{
	return *mTimer;
}