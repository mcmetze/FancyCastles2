#ifndef METZE_GAME_MANAGER_H
#define METZE_GAME_MANAGER_H

#include "BoardRenderer.h"

class InputHandler;

class GameManager
{
public:
	GameManager(const int&, std::unique_ptr<BoardRenderer> renderer, std::unique_ptr<InputHandler> input);

	void GameLoop();

	void UpdateSelectedTilePosition(int delta_q, int delta_r);

	void DoPick();

	void ExitGame() { mRunGameLoop = false; }

private:
	void CreateGameBoard();

	void SetupRenderer();
	void SetupTiles();

	void AssignPlayers();
	void SetupPlayers();

	void PrintTileInfo(int tileID);

	Color GetVertexColorFromType(const ResourceType& tileType);

	std::unique_ptr<BoardRenderer> mRenderComponent;
	std::unique_ptr<InputHandler> mInputHandler;
	std::unique_ptr<Board> mGameBoard;
	
	int mNumPlayers;

	AxialCoord mSelectedTilePos;
	int mTileIndexPicked;
	std::unordered_map<AxialCoord, int, AxialHash> mChosenTilesMap;
	
	bool mRunGameLoop;

	bool mDebugPrint;
};

#endif