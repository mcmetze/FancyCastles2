#include "GameManager.h"
#include "InputHandler.h"
#include "Player.h"

GameManager::GameManager(const int& numPlayers, std::unique_ptr<BoardRenderer> renderer, std::unique_ptr<InputHandler> inputhandler)
	: mNumPlayers(numPlayers)
	, mRenderComponent(std::move(renderer))
	, mInputHandler(std::move(inputhandler))
	, mRunGameLoop(true)
	, mDebugPrint(false)
{
	ClearSelection();

	CreatePlayers();
	CreateGameBoard();
	SetupRenderer();

	mInputHandler->SetGameManager(this);
}

GameManager::~GameManager()
{
}

void
GameManager::CreateGameBoard()
{
	mGameBoard = std::make_unique<Board>();
	mGameBoard->MakeBoard(mNumPlayers);
}

void 
GameManager::CreatePlayers()
{
	for (int playerID = 0; playerID < mNumPlayers; ++playerID)
	{
		mPlayerMap.emplace(playerID, std::make_unique<Player>(playerID));
	}
}

void 
GameManager::SetupTiles()
{
	std::vector<AxialCoord> tileCoords;	
	std::vector<Color> tileColors;
	for (int tileIndex = 0; tileIndex < mGameBoard->GetNumTiles(); ++tileIndex)
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
			mPlayerMap[curPlayerID]->AddTile(tileIndex);
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
GameManager::PrintTileInfo(const int&  tileID)
{
	mGameBoard->PrintTileInfo(tileID);
}

void
GameManager::ClearSelection()
{
	mSelectedTilePos = { INT_MAX, INT_MAX };
	mTileIndexPicked = INT_MAX;
	mRenderComponent->SetSelection(mSelectedTilePos);
}

void 
GameManager::UpdateCurrentTileSelection(const AxialCoord& pos)
{
	if ( mGameBoard->IsPositionValid(pos) )
	{
		mSelectedTilePos = pos;
		mTileIndexPicked = mGameBoard->GetTileIndex(pos);
		mRenderComponent->SetSelection(mSelectedTilePos);

		if ( mDebugPrint )
			PrintTileInfo(mTileIndexPicked);
	}
}

void
GameManager::SelectTileFromMouse()
{
	const auto pickedIndex = mRenderComponent->DoPick();
	if (pickedIndex >= 0 && pickedIndex <= mGameBoard->GetNumTiles())
	{
		UpdateCurrentTileSelection(mGameBoard->GetTileCoord(pickedIndex));
	}
	else
	{
		ClearSelection();
	}
}

void 
GameManager::MoveTileSelection(const int& delta_q, const int&  delta_r)
{
	AxialCoord moved(mSelectedTilePos.r + delta_r, mSelectedTilePos.q + delta_q);
	UpdateCurrentTileSelection( moved );
}

void
GameManager::HarvestResource()
{
	if ( mGameBoard->IsPositionValid(mSelectedTilePos) && mGameBoard->GetTileType(mTileIndexPicked) != WATER )
	{
		const auto tileOwnerID = mGameBoard->GetTileOwner(mTileIndexPicked);
		const auto& player = mPlayerMap[tileOwnerID];
		if (player && player->SetTimerLocation(mTileIndexPicked, mGameBoard->GetHarvestRate(mTileIndexPicked)))
		{
			player->StartHarvest();
		}
	}
}

void
GameManager::GameLoop()
{
	while (mRunGameLoop)
	{
		mRenderComponent->RenderScene();
		
		for (const auto& curPlayer : mPlayerMap)
			curPlayer.second->Tick();
	}

	mRenderComponent->Cleanup();
}
