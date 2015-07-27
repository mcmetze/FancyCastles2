#include "GameManager.h"

#include "Board.h"
#include "BoardRenderer.h"
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
	mRenderComponent->Cleanup();
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
			mPlayerMap[curPlayerID]->TakeTileOwnership(tileIndex);
			mGameBoard->SetTileOwner(tileIndex, curPlayerID);
			curPlayerID = (curPlayerID + 1) % mNumPlayers;
		}
	}
}

void
GameManager::SetupPlayers()
{
	int curPlayerID = 0;
	const int numTilesToSelect = mNumPlayers*(NUMTYPES - 1);

	std::unordered_map<AxialCoord, int> chosenTilesMap;
	while ( chosenTilesMap.size() < numTilesToSelect )
	{
		if ( mGameBoard->IsPositionValid(mSelectedTilePos) && chosenTilesMap.find(mSelectedTilePos) == chosenTilesMap.end() )
		{
			if ( mGameBoard->GetTileType(mTileIndexPicked) != WATER )
			{
				printf("player %i selected tile (%i, %i)\n", curPlayerID, mSelectedTilePos.r, mSelectedTilePos.q);
				mPlayerMap[curPlayerID]->TakeTileOwnership(mTileIndexPicked);
				mGameBoard->SetTileOwner(mTileIndexPicked, curPlayerID);
				chosenTilesMap[mSelectedTilePos] = curPlayerID;
				curPlayerID = (curPlayerID + 1) % mNumPlayers;
			}
		}

		//the game loop isnt running yet, so the selection texture wont render without this
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
	if ( pickedIndex >= 0 && pickedIndex <= mGameBoard->GetNumTiles() )
	{
		UpdateCurrentTileSelection(mGameBoard->GetTileCoord(pickedIndex));
	}
	else
	{
		ClearSelection();
	}
}

void 
GameManager::MoveTileSelection(const AxialCoord& offset)
{
	const AxialCoord moved = mSelectedTilePos + offset;
	UpdateCurrentTileSelection( moved );
}

void
GameManager::HarvestResource()
{
	if ( mGameBoard->IsPositionValid(mSelectedTilePos) && mGameBoard->GetTileType(mTileIndexPicked) != WATER )
	{
		const auto tileOwnerID = mGameBoard->GetTileOwner(mTileIndexPicked);
		const auto& player = mPlayerMap[tileOwnerID];
		if ( player && player->SetTimerLocation(mTileIndexPicked, mGameBoard->GetHarvestRate(mTileIndexPicked)) )
		{
			player->StartHarvest();
		}
	}
}

void //todo- return the map
GameManager::GetAllResourcesAccessibleFromTile(const int& tileIndex)
{
	const auto tileOwnerID = mGameBoard->GetTileOwner(tileIndex);
	const auto playerIter = mPlayerMap.find(tileOwnerID);
	if (playerIter == mPlayerMap.end())
		return;

	const auto& player = playerIter->second;
	std::unordered_map<ResourceType, int> resourcesTotals;
	for (const auto& tile : mGameBoard->FindConnectedTilesWithSameOwner(tileOwnerID, tileIndex))
	{
		const int numResources = player->GetRawResourcesOnTile(tile);
		resourcesTotals[mGameBoard->GetTileType(tile)] += numResources;
	}

	printf("tile %i, player %i :\n", tileIndex, tileOwnerID);
	for (const auto& resource : resourcesTotals)
	{
		printf("\tresource %i count: %i\n", resource.first, resource.second);
	}
}

void
GameManager::Build()
{
	if ( mGameBoard->IsPositionValid(mSelectedTilePos) )
	{
		GetAllResourcesAccessibleFromTile( mGameBoard->GetTileIndex(mSelectedTilePos) );
		//todo - validate resources the player has vs. what they need to build
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
}
