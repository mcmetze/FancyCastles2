#include "PlayerController.h"

#include <assert.h>

#include "GameObject.h"
#include "Timer.h"
#include "Player.h"

PlayerController::PlayerController(PlayerList& players)
{
	for (auto& p : players)
	{
		mPlayerMap[p->GetPlayerID()] = std::move(p);
	}
}

PlayerController::~PlayerController()
{
}

void
PlayerController::ObserveTimers(ObserverPtr obs)
{
	for (auto& player : mPlayerMap)
	{
		player.second->GetTimer().AddObserver(obs);
	}
}

void
PlayerController::Tick()
{
	for (const auto& player : mPlayerMap)
	{
		player.second->GetTimer().Tick();
	}
}

Player&
PlayerController::GetPlayer(int playerID)
{
	const auto playerHandle = mPlayerMap.find(playerID);
	assert(playerHandle != mPlayerMap.end());

	return *playerHandle->second;
}

const Player&
PlayerController::GetConstPlayer(int playerID) const
{
	const auto playerHandle = mPlayerMap.find(playerID);
	assert(playerHandle != mPlayerMap.end());

	return *playerHandle->second;
}

TileIDSet
PlayerController::GetPlayerTiles(int playerID) const
{
	return GetConstPlayer(playerID).GetPlayerTileIDs();
}

void
PlayerController::AddTileToPlayer(int tileID, int playerID)
{
	GetPlayer(playerID).AddTile(tileID);
}

void
PlayerController::RemoveTileFromPlayer(int tileID, int playerID)
{
	GetPlayer(playerID).RemoveTile(tileID);
}

GameObjectSet
PlayerController::GetGameObjectsFromTiles(int playerID, const TileIDSet& tiles) const
{
	GameObjectSet objectsFromTiles;
	for (const auto& obj : GetConstPlayer(playerID).GetGameObjects())
	{
		if (tiles.count(obj->GetPosition()) > 0)
			objectsFromTiles.insert(obj);
	}

	return objectsFromTiles;
}

bool
PlayerController::MovePlayerTimer(int playerID, int selectedTileID)
{
	auto& player = GetPlayer(playerID);
	if (!player.OwnsTile(selectedTileID))
		return false; //$TODO this could be legal

	auto& playerTimer = player.GetTimer();
	if (playerTimer.IsBusy())
		return false;

	playerTimer.SetPosition(selectedTileID);

	return true;
}

void
PlayerController::FlipPlayerTimer(int playerID, TimerResultPtr result)
{
	GetPlayer(playerID).GetTimer().OnTimerStart(result);
}

void
PlayerController::AddGameObjectToPlayer(GameObjectPtr obj, int playerID)
{
	GetPlayer(playerID).AddGameObject(obj);
}

void 
PlayerController::GiveBills(int playerIDOfGiver, int playerIDOfTaker, int amount)
{
	auto& givingPlayer = GetPlayer(playerIDOfGiver);
	auto& takingPlayer = GetPlayer(playerIDOfTaker);

	const auto numBillsCanGive = givingPlayer.GetNumBills();
	if (numBillsCanGive < amount)
		return;

	givingPlayer.TakeBills(amount);
	takingPlayer.AddBills(amount);
}