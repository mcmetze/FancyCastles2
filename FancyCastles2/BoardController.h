#pragma once

#include <unordered_map>
#include <unordered_set>

#include "TileTraits.h"

class Board;

using BoardPtr = std::unique_ptr < Board > ;
using TileIDList = std::vector < int > ;
using TileIDSet = std::unordered_set < int > ;

class BoardController
{
public:
	BoardController() = delete;
	BoardController(const BoardController&) = delete;
	BoardController& operator=(const BoardController& rhs) = delete;

	BoardController(BoardPtr board);
	~BoardController();

	int GetSelectedTileForPlayer(int playerID) const;
	void SetSelectedTileForPlayer(int playerID, int tileID);

	AxialCoord GetSelectionCoordsForPlayer(int playerID) const;
	void MoveSelectionCoordsForPlayer(int playerID, AxialCoord delta);

	int GetHarvestRate(int tileID) const;
	ResourceType GetTileType(int tileID) const;

	TileIDSet FindNeighbors(int tileID) const;
	TileIDSet FindConnectedComponent(const TileIDSet& tiles, int source) const;
	TileIDList FindConnectedPath(const TileIDSet& tiles, int source, int target) const;

private:

	BoardPtr mBoard;

	std::unordered_map<int, int> mPlayerSelectionsMap;
};