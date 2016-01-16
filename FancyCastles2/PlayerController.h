#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Observer.h"

class GameObject;
class Observer;
class Player;

using GameObjectPtr = std::shared_ptr < GameObject >;
using GameObjectSet = std::unordered_set < GameObjectPtr >;
using PlayerPtr = std::unique_ptr < Player > ;
using PlayerList = std::vector < PlayerPtr >;
using PlayerMap = std::unordered_map < int, PlayerPtr > ;
using TileIDSet = std::unordered_set < int >;

class PlayerController
{
public:
	PlayerController(const PlayerController&) = delete;
	PlayerController& operator=(const PlayerController& rhs) = delete;

	PlayerController(PlayerList& players);
	~PlayerController();

	void ObserveTimers(ObserverPtr obs);

	void Tick();

	void FlipPlayerTimer(int playerID, TimerResultPtr result);
	void CancelPlayerTimer(int playerID);
	bool MovePlayerTimer(int playerID, int selectedTileID);

	void AddGameObjectToPlayer(GameObjectPtr obj, int playerID);
	GameObjectSet GetGameObjectsFromTiles(int playerID, const TileIDSet& tiles) const;

	TileIDSet GetPlayerTiles(int playerID) const;
	void AddTileToPlayer(int tileID, int playerID);
	void RemoveTileFromPlayer(int tileID, int playerID);

	void GiveBills(int playerIDOfGiver, int playerIDOfTaker, int amount);

private:
	Player& GetPlayer(int playerID);
	const Player& GetConstPlayer(int playerID) const;

	PlayerMap mPlayerMap;
};