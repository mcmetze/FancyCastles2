#include "GameManager.h"
#include "InputHandler.h"

GameManager::GameManager(const int& numPlayers, std::unique_ptr<BoardRenderer> renderer, std::unique_ptr<InputHandler> inputhandler)
	: mNumPlayers(numPlayers)
	, mRenderComponent(std::move(renderer))
	, mInputHandler(std::move(inputhandler))
	, mRunGameLoop(true)
	, mSelectedTilePos(INT_MAX, INT_MAX)
	, mTileIndexPicked(INT_MAX)
	, mDebugPrint(true)
{
	CreateGameBoard();
	SetupRenderer();
	//SetupPlayers();
	AssignPlayers();
}

void
GameManager::CreateGameBoard()
{
	mGameBoard = std::make_unique<Board>();
	mGameBoard->MakeBoard(mNumPlayers);
}

void 
GameManager::SetupTiles()
{
	std::vector<AxialCoord> tileCoords;	
	std::vector<Color> tileColors;
	for (size_t tileIndex = 0; tileIndex < mGameBoard->GetNumTiles(); ++tileIndex)
	{
		tileCoords.push_back(mGameBoard->GetTileCoord(tileIndex));
		tileColors.push_back(GetVertexColorFromType(mGameBoard->GetTileType(tileIndex)));
	}
	mRenderComponent->SetupTileColors(tileColors);
	mRenderComponent->SetupHexVerts(tileCoords);
}

Color
GameManager::GetVertexColorFromType(const ResourceType& tileType)
{
	//placeholder method to verify
	Color tileColor;
	switch (tileType)
	{
	case WATER:
		tileColor.b = 1.f;
		break;
	case GRASS:
		tileColor.g = 1.f;
		break;
	case WHEAT:
		tileColor.r = 1.f;
		tileColor.g = 1.f;
		break;
	case TREE:
		tileColor.r = 0.6f;
		tileColor.g = 0.29f;
		break;
	case ORE:
		tileColor.r = 0.47f;
		tileColor.g = 0.47f;
		tileColor.b = 0.47f;
		break;
	default:
		break;
	}

	return tileColor;
}

void 
GameManager::SetupRenderer()
{
	SetupTiles();
	mRenderComponent->SetupBuffers();
	mRenderComponent->SetupShaders();
	mRenderComponent->SetupAttributes();
	mRenderComponent->SetupTexture("TileOutline.png");
	mRenderComponent->SetSelection(mSelectedTilePos);
}

void 
GameManager::AssignPlayers()
{
	int curPlayerID = 0;

	for (int tileIndex = 0; tileIndex < mGameBoard->GetNumTiles(); ++tileIndex)
	{
		if (mGameBoard->GetTileType(tileIndex) != WATER)
		{
			mSelectedTilePos = mGameBoard->GetTileCoord(tileIndex);

			printf("player %i assigned to tile (%i, %i)\n", curPlayerID, mSelectedTilePos.r, mSelectedTilePos.q);
			mGameBoard->SetTileOwner(tileIndex, curPlayerID);
			curPlayerID = (curPlayerID + 1) % mNumPlayers;
		}
	}

	mRenderComponent->RenderScene();
}

void
GameManager::SetupPlayers()
{
	int curPlayerID = 0;
	const int numTilesToSelect = mNumPlayers*(NUMTYPES - 1);

	while (mChosenTilesMap.size() < numTilesToSelect)
	{
		Command* mouseCmd = mInputHandler->HandleMouseClick();
		mouseCmd->Execute(this);

		if (mGameBoard->IsPositionValid(mSelectedTilePos) && mChosenTilesMap.find(mSelectedTilePos) == mChosenTilesMap.end())
		{
			if (mGameBoard->GetTileType(mTileIndexPicked) != WATER)
			{
				printf("player %i selected tile (%i, %i)\n", curPlayerID, mSelectedTilePos.r, mSelectedTilePos.q);

				mGameBoard->SetTileOwner(mTileIndexPicked, curPlayerID);
				mChosenTilesMap[mSelectedTilePos] = curPlayerID;
				curPlayerID = (curPlayerID + 1) % mNumPlayers;
			}
		}

		mRenderComponent->RenderScene();
	}
}

void
GameManager::PrintTileInfo(int tileID)
{
	mGameBoard->PrintTileInfo(tileID);
}

void
GameManager::DoPick()
{
	mTileIndexPicked = mRenderComponent->DoPick();
	if (mTileIndexPicked <= mGameBoard->GetNumTiles())
	{
		mSelectedTilePos = mGameBoard->GetTileCoord(mTileIndexPicked);
		if (mDebugPrint)
			PrintTileInfo(mTileIndexPicked);
	}
	else
	{
		mSelectedTilePos = { INT_MAX, INT_MAX };
	}
}

void 
GameManager::UpdateSelectedTilePosition(int delta_q, int delta_r)
{
	AxialCoord moved(mSelectedTilePos.r + delta_r, mSelectedTilePos.q + delta_q);

	if (mGameBoard->IsPositionValid(moved))
	{
		mSelectedTilePos = moved;
	}
}

void
GameManager::GameLoop()
{
	while (mRunGameLoop)
	{
		Command* keyCmd = mInputHandler->HandleKeyPress();
		keyCmd->Execute(this);

		Command* mouseCmd = mInputHandler->HandleMouseClick();
		mouseCmd->Execute(this);

		mRenderComponent->SetSelection(mSelectedTilePos);

		//update list of entities - players, timers, etc.

		mRenderComponent->RenderScene();
	}

	mRenderComponent->Cleanup();
}