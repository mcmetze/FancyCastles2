#pragma once

#include <unordered_map>

#include "TileTraits.h"

class Board;
class BoardRenderer;
class InputHandler;
class Player;

class GameManager
{
public:
	GameManager(const int& numPlayers, std::unique_ptr<BoardRenderer> renderer);
	~GameManager();

	//Game control methods
	void GameLoop();
	void ExitGame();

	//Player setup methods
	void SetupPlayers();
	void AssignPlayers();

	//Selection related methods
	void MoveTileSelection(const AxialCoord& offset);
	void SelectTileFromMouse();

	//Player action methods
	void HarvestResource();
	void Build();

private:
	//Creation helper methods
	void CreateGameBoard();
	void CreatePlayers();

	//Rendering related methods
	Color GetVertexColorFromType(const ResourceType& tileType);
	void SetupRenderer();

	//Tile related methods
	void SetupTiles();
	void GetAllResourcesAccessibleFromTile(const int& tileIndex);
	void PrintTileInfo(const int& tileID) const;


	std::unique_ptr<BoardRenderer> mRenderComponent;
	std::unique_ptr<Board> mGameBoard;
	
	int mNumPlayers;
	std::unordered_map<int, std::unique_ptr<Player> > mPlayerMap;

	TileSelection mSelection;
	
	bool mRunGameLoop;
};