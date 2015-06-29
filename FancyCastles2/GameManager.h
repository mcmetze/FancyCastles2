#ifndef METZE_GAME_MANAGER_H
#define METZE_GAME_MANAGER_H

#include "BoardRenderer.h"

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

	void MoveTileSelection(const int& delta_q, const int& delta_r);
	void SelectTileFromMouse();

	void HarvestResource();
	void ExitGame() { mRunGameLoop = false; }

private:
	void CreateGameBoard();
	void CreatePlayers();

	void SetupRenderer();
	void SetupTiles();

	void ClearSelection();
	void UpdateCurrentTileSelection(const AxialCoord& pos);

	void PrintTileInfo(const int& tileID);

	Color GetVertexColorFromType(const ResourceType& tileType);

	std::unique_ptr<BoardRenderer> mRenderComponent;
	std::unique_ptr<InputHandler> mInputHandler;
	std::unique_ptr<Board> mGameBoard;
	
	int mNumPlayers;
	std::unordered_map<int, std::unique_ptr<Player> > mPlayerMap;

	AxialCoord mSelectedTilePos;
	int mTileIndexPicked;
	std::unordered_map<AxialCoord, int> mChosenTilesMap;
	
	bool mRunGameLoop;

	bool mDebugPrint;
};

#endif