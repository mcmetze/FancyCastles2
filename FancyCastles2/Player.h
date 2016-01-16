#pragma once

#include <memory>
#include <unordered_set>

class GameObject;
class TimerObject;
enum class GameObjectType;

using GameObjectPtr = std::shared_ptr < GameObject >;
using GameObjectSet = std::unordered_set < GameObjectPtr >;
using TileIDSet = std::unordered_set < int >;
using TimerPtr = std::unique_ptr < TimerObject > ;

class Player
{
public:
	Player(int playerID, TimerPtr timer, int numBills);

	int GetPlayerID() const;

	void AddBills(int numBillsToAdd);
	void TakeBills(int numBillsToTake);
	int GetNumBills() const;

	void AddTile(int tileID);
	void RemoveTile(int tileID);
	bool OwnsTile(int tileID) const;
	TileIDSet GetPlayerTileIDs() const;

	void AddGameObject(GameObjectPtr obj);
	void RemoveGameObject(GameObjectPtr obj);
	GameObjectSet GetGameObjects() const;

	TimerObject& GetTimer();

private:
	int mPlayerID;
	int mNumBills;

	TileIDSet mPlayerTileIDs;
	GameObjectSet mPlayerObjects;
	TimerPtr mTimer;
};