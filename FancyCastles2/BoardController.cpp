#include "BoardController.h"

#include <queue>

#include "Board.h"

BoardController::BoardController(BoardPtr board) : mBoard(std::move(board))
{
}

BoardController::~BoardController()
{
}


int
BoardController::GetSelectedTileForPlayer(int playerID) const
{
	auto mapEntry = mPlayerSelectionsMap.find(playerID);
	if (mapEntry == mPlayerSelectionsMap.end())
		return -1;

	return mapEntry->second;
}

AxialCoord
BoardController::GetSelectionCoordsForPlayer(int playerID) const
{
	const auto selectedTileID = GetSelectedTileForPlayer(playerID);
	if (!mBoard->IsTileValid(selectedTileID))
		return AxialCoord();

	return mBoard->GetTileCoord(selectedTileID);
}
void
BoardController::MoveSelectionCoordsForPlayer(int playerID, AxialCoord delta)
{
	const auto selectedTileID = GetSelectedTileForPlayer(playerID);
	if (!mBoard->IsTileValid(selectedTileID))
		return;

	const auto selectedTileCoords = mBoard->GetTileCoord(selectedTileID);
	const auto movedSelectionCoords = selectedTileCoords + delta;
	if (!mBoard->IsPositionValid(movedSelectionCoords))
		return;

	mPlayerSelectionsMap[playerID] = mBoard->GetTileIndex(movedSelectionCoords);
}

void
BoardController::SetSelectedTileForPlayer(int playerID, int tileID)
{
	if (mBoard->IsTileValid(tileID))
		mPlayerSelectionsMap[playerID] = tileID;
}

int
BoardController::GetHarvestRate(int tileID) const
{
	if (mBoard->IsTileValid(tileID))
		return mBoard->GetHarvestRate(tileID);

	return 0;
}

ResourceType
BoardController::GetTileType(int tileID) const
{
	if (mBoard->IsTileValid(tileID))
		return mBoard->GetTileType(tileID);

	return ResourceType::INVALID;
}

const std::vector<AxialCoord> adjacentOffsets =
{
	{ -1, 0 }, { -1, 1 }, { 0, -1 },
	{ 1, 0 }, { 1, -1 }, { 0, 1 }
};

TileIDSet
BoardController::FindNeighbors(int tileID) const
{
	TileIDSet neighbors;

	const auto position = mBoard->GetTileCoord(tileID);
	for (auto& offset : adjacentOffsets)
	{
		AxialCoord neighbor = position + offset;
		if (mBoard->IsPositionValid(neighbor))
			neighbors.insert(mBoard->GetTileIndex(neighbor));
	}

	return neighbors;
}

TileIDList
BoardController::FindConnectedPath(const TileIDSet& tiles, int source, int target) const
{
	TileIDList path;

	return path;
}

TileIDSet
BoardController::FindConnectedComponent(const TileIDSet& tiles, int source) const
{
	TileIDSet connectedComponent;
	std::queue<int> tileIDsToVisit;

	//make sure the source tile is in the set we are searching
	if (tiles.count(source) > 0)
	{
		connectedComponent.insert(source);
		tileIDsToVisit.push(source);
	}

	while (!tileIDsToVisit.empty())
	{
		const auto curTileID = tileIDsToVisit.front();
		tileIDsToVisit.pop();

		for (auto neighbor : FindNeighbors(curTileID))
		{
			//only add neighboring tiles that belong to the set we care about and haven't seen yet
			if (tiles.count(neighbor) > 0 && connectedComponent.count(neighbor) == 0)
			{
				connectedComponent.insert(neighbor);
				tileIDsToVisit.push(neighbor);

				//stop looking if we found all tiles in the set
				if (connectedComponent.size() == tiles.size())
					return connectedComponent;
			}
		}
	}

	return connectedComponent;
}