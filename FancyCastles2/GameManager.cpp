#include "GameManager.h"

#include "BoardController.h"
#include "BoardRenderer.h"
#include "Commands.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Timer.h"

GameManager::GameManager(BoardRendererPtr renderComponent, BoardControllerPtr boardController, PlayerControllerPtr playerController)	
	: mRenderComponent(std::move(renderComponent))
	, mBoardController(std::move(boardController))
	, mPlayerController(playerController)
	, mNextObjectID(0)
	, mCurPlayerChoosing(1)
{
}

GameManager::~GameManager()
{
	mRenderComponent->Cleanup();
}

void
GameManager::OnNotify(Command* cmd)
{
	auto moveSelectionCmd = dynamic_cast<MoveSelectionCommand*>(cmd);
	if (moveSelectionCmd)
	{
		MoveTileSelection(mCurPlayerChoosing, moveSelectionCmd->GetOffset());
		return;
	}

	auto pickCmd = dynamic_cast<PickSelectionCommand*>(cmd);
	if (pickCmd)
	{
		SelectTileFromMouse(mCurPlayerChoosing);
		return;
	}

	auto harvestCmd = dynamic_cast<HarvestCommand*>(cmd);
	if (harvestCmd)
	{
		//move the timer to the player's selected tile
		const auto harvestLocation = mBoardController->GetSelectedTileForPlayer(mCurPlayerChoosing);
		if (!mPlayerController->MovePlayerTimer(mCurPlayerChoosing, harvestLocation))
			return;

		//setup the relevant info needed to create the resource when the timer is done
		const auto quantity = mBoardController->GetHarvestRate(harvestLocation);
		auto result = std::make_shared<TimerResult>(GameObjectType::RESOURCE, harvestLocation, quantity, mCurPlayerChoosing);
		
		//start the timer
		mPlayerController->FlipPlayerTimer(mCurPlayerChoosing, result);
		return;
	}

	auto buildCmd = dynamic_cast<BuildCommand*>(cmd);
	if (buildCmd)
	{
		const auto playerSelection = mBoardController->GetSelectedTileForPlayer(mCurPlayerChoosing);
		const auto playerTiles = mPlayerController->GetPlayerTiles(mCurPlayerChoosing);
		const auto& playerConnectedTiles = mBoardController->FindConnectedComponent(playerTiles, playerSelection);
		const auto& availableObjects = mPlayerController->GetGameObjectsFromTiles(mCurPlayerChoosing, playerConnectedTiles);

		//$TODO get requested build object from parameter bag
		//$TODO validate availableObjects meet build requirements

		return;
	}

	//$TODO just a hack during development to emulate multiple players
	//eventually, this will be grabbed from some parameter of the command
	auto changePlayerCmd = dynamic_cast<ChangePlayerCommand*>(cmd);
	if (changePlayerCmd)
	{
		const auto requestedPlayer = changePlayerCmd->GetPlayerID();
		if (mPlayerController->GetPlayerTiles(requestedPlayer).empty())
			return;

		mCurPlayerChoosing = requestedPlayer;
		mRenderComponent->SetSelection(mBoardController->GetSelectionCoordsForPlayer(mCurPlayerChoosing));
		return;
	}

	auto exitCmd = dynamic_cast<ExitGameCommand*>(cmd);
	if (exitCmd)
	{
		StopGame();
		return;
	}
}

void
GameManager::OnNotify(TimerResultPtr result)
{
	// Someone's timer finished. Create everything and hand it off to the player
	for (int i = 0; i < result->mQuantity; ++i)
	{
		GameObjectPtr resultObject = nullptr;

		switch (result->mResultObjectType)
		{
		case GameObjectType::RESOURCE:
			resultObject = CreateResourceGameObject(*result);
			break;
		}

		if (resultObject != nullptr)
			mPlayerController->AddGameObjectToPlayer(resultObject, result->mPlayerID);
	}

	//$TODO also let the renderer know stuff changed
}

GameObjectPtr
GameManager::CreateResourceGameObject(const TimerResult& result)
{
	const auto resourceType = mBoardController->GetTileType(result.mResultLocation);
	const auto objectID = GetNextObjectID();
	assert(objectID > 0);

	return std::make_shared<ResourceObject>(objectID, result.mResultLocation, resourceType);
}

int
GameManager::GetNextObjectID()
{
	if (mNextObjectID + 1 > mNextObjectID)
	{
		mNextObjectID++;
		return mNextObjectID;
	}

	//$TODO will need to try to find an open id
	return -1;
}

void
GameManager::SelectTileFromMouse(int playerID)
{
	const auto pickedTile = mRenderComponent->DoPick();

	mBoardController->SetSelectedTileForPlayer(playerID, pickedTile);
	mRenderComponent->SetSelection(mBoardController->GetSelectionCoordsForPlayer(playerID));
}

void 
GameManager::MoveTileSelection(int playerID, const AxialCoord& offset)
{
	mBoardController->MoveSelectionCoordsForPlayer(playerID, offset);
	mRenderComponent->SetSelection(mBoardController->GetSelectionCoordsForPlayer(playerID));
}

void
GameManager::GameLoop()
{
	while (mRunGameLoop)
	{
		mRenderComponent->RenderScene();
		mPlayerController->Tick();
	}
}

void
GameManager::StartGame()
{
	mRunGameLoop = true;
	GameLoop();
}

void
GameManager::StopGame()
{
	mRunGameLoop = false;
}