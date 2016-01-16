#pragma once

#include <unordered_map>

#include "TileTraits.h"
#include "Observer.h"

class BoardRenderer;
class BoardController;
class GameObject;
class PlayerController;

struct TimerResult;

using BoardRendererPtr = std::unique_ptr<BoardRenderer>;
using BoardControllerPtr = std::unique_ptr<BoardController>;
using GameObjectPtr = std::shared_ptr < GameObject > ;
using PlayerControllerPtr = std::shared_ptr<PlayerController>;

class GameManager : public Observer
{
public:
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager& rhs) = delete;

	GameManager(BoardRendererPtr renderComponent, BoardControllerPtr boardController, PlayerControllerPtr playerController);
	~GameManager();

	void OnNotify(TimerResultPtr result) override;
	void OnNotify(Command* cmd) override;

	void StartGame();
	void StopGame();

private:
	void GameLoop();
	
	GameObjectPtr CreateResourceGameObject(const TimerResult& result);
	void MoveTileSelection(int playerID, const AxialCoord& offset);
	void SelectTileFromMouse(int playerID);

	int GetNextObjectID();
	
private:
	BoardControllerPtr mBoardController;
	BoardRendererPtr mRenderComponent;
	PlayerControllerPtr mPlayerController;

	int mNextObjectID;

	int mCurPlayerChoosing;
	bool mRunGameLoop;
};