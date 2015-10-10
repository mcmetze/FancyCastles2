#include "GameManager.h"

#include "Board.h"
#include "BoardRenderer.h"
#include "Player.h"

#include <iostream>

GameManager::GameManager(const int& numPlayers, std::unique_ptr<BoardRenderer> renderer)
	: mNumPlayers(numPlayers)
	, mRenderComponent(std::move(renderer))
	, mRunGameLoop(true)
{
	CreatePlayers();
	CreateGameBoard();
	SetupRenderer();

	mSelection.SetBoard(mGameBoard.get());
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
		tileColor.r = tileColor.g = 1.f;
		break;
	case TREE:
		tileColor.r = 0.6f;
		tileColor.g = 0.29f;
		break;
	case ORE:
		tileColor.r = tileColor.g = tileColor.b = 0.47f;
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
	mRenderComponent->SetSelection(mSelection.GetSelectionPos());
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
GameManager::PrintTileInfo(const int&  tileID) const
{
	mGameBoard->PrintTileInfo(tileID);
}

void
GameManager::SelectTileFromMouse()
{
	const auto pickedIndex = mRenderComponent->DoPick();
	mSelection.Update(pickedIndex);
	if ( !mSelection.IsValid() )
		mSelection.Clear();
	
	mRenderComponent->SetSelection(mSelection.GetSelectionPos());
}

void 
GameManager::MoveTileSelection(const AxialCoord& offset)
{
	const AxialCoord moved = mSelection.GetSelectionPos() + offset;
	mSelection.Update(moved);
	if ( !mSelection.IsValid() )
		return;

	mRenderComponent->SetSelection(mSelection.GetSelectionPos());
}

void
GameManager::HarvestResource()
{
	if ( !mSelection.IsValid() )
		return;
	
	const auto& selectedIndex = mSelection.GetSelectionIndex();
	if (mGameBoard->GetTileType(selectedIndex) != WATER)
	{
		const auto tileOwnerID = mGameBoard->GetTileOwner(selectedIndex);
		const auto& player = mPlayerMap[tileOwnerID];
		if ( player && player->SetTimerLocation(selectedIndex, mGameBoard->GetHarvestRate(selectedIndex)) )
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
	if (!mSelection.IsValid())
		return;

	GetAllResourcesAccessibleFromTile( mSelection.GetSelectionIndex() );
	//todo - validate resources the player has vs. what they need to build
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

void
GameManager::ExitGame()
{
	mRunGameLoop = false;
}