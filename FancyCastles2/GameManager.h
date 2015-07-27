#pragma once

#include <unordered_map>

#include "TileTraits.h"

class Board;
class BoardRenderer;
class Command;
class InputHandler;
class Player;

class GameManager
{
public:
	GameManager(const int&, std::unique_ptr<BoardRenderer> renderer, std::unique_ptr<InputHandler> input);
	~GameManager();

	void GameLoop();

	void SetupPlayers();
	void AssignPlayers();

	void MoveTileSelection(const AxialCoord& offset);
	void SelectTileFromMouse();

	void HarvestResource();
	void Build();

	void ExitGame() { mRunGameLoop = false; }

private:
	void CreateGameBoard();
	void CreatePlayers();

	void SetupRenderer();
	void SetupTiles();

	void ClearSelection();
	void UpdateCurrentTileSelection(const AxialCoord& pos);

	void GetAllResourcesAccessibleFromTile(const int& tileIndex);

	void PrintTileInfo(const int& tileID);

	Color GetVertexColorFromType(const ResourceType& tileType);

	std::unique_ptr<BoardRenderer> mRenderComponent;
	std::unique_ptr<InputHandler> mInputHandler;
	std::unique_ptr<Board> mGameBoard;
	
	int mNumPlayers;
	std::unordered_map<int, std::unique_ptr<Player> > mPlayerMap;

	AxialCoord mSelectedTilePos;
	int mTileIndexPicked;
	
	bool mRunGameLoop;

	bool mDebugPrint;
};