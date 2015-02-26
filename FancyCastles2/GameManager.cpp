#include "GameManager.h"
#include "InputHandler.h"

GameManager::GameManager(const int& numPlayers, std::unique_ptr<BoardRenderer> renderer, std::unique_ptr<InputHandler> inputhandler)
	: mNumPlayers(numPlayers)
	, mRenderComponent(std::move(renderer))
	, mInputHandler(std::move(inputhandler))
	, mRunGameLoop(true)
{
	CreateGameBoard();
	SetupRenderer();
}

void
GameManager::CreateGameBoard()
{
	mGameBoard = std::make_unique<Board>();
	mGameBoard->MakeBoard(mNumPlayers);
}

void 
GameManager::SetupHexVerts()
{
	std::vector<AxialCoord> tileCoords;
	for (size_t tileIndex = 0; tileIndex < mGameBoard->GetNumTiles(); ++tileIndex)
	{
		tileCoords.push_back(mGameBoard->GetTileCoord(tileIndex));
	}
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
GameManager::SetupTileColors()
{
	std::vector<Color> tileColors;
	for (size_t tileIndex = 0; tileIndex < mGameBoard->GetNumTiles(); ++tileIndex)
	{
		tileColors.push_back(GetVertexColorFromType(mGameBoard->GetTileType(tileIndex)));
	}
	mRenderComponent->SetupTileColors(tileColors);
}

void 
GameManager::SetupRenderer()
{
	SetupHexVerts();
	SetupTileColors();
	mRenderComponent->SetupBuffers();
	mRenderComponent->SetupShaders();
	mRenderComponent->SetupAttributes();
	mRenderComponent->SetupTexture("TileOutline.png");
}

void
GameManager::PrintTileInfo(int tileID)
{
	printf("position {%i, %i}\n", mGameBoard->GetTileCoord(tileID).r, mGameBoard->GetTileCoord(tileID).q);
	switch (mGameBoard->GetTileType(tileID))
	{
	case WATER:
		printf("water\n");
		break;
	case GRASS:
		printf("grass\n");
		break;
	case WHEAT:
		printf("wheat\n");
		break;
	case ORE:
		printf("ore\n");
		break;
	case TREE:
		printf("tree\n");
		break;
	default:
		break;
	}
}

void
GameManager::DoPick()
{
	auto tileIndexPicked = mRenderComponent->DoPick();
	if (tileIndexPicked <= mGameBoard->GetNumTiles())
	{
		PrintTileInfo(tileIndexPicked);
		mSelectedTilePos = mGameBoard->GetTileCoord(tileIndexPicked);
	}
}

void 
GameManager::UpdateSelectedTilePosition(int delta_q, int delta_r)
{
	mSelectedTilePos.q += delta_q;
	mSelectedTilePos.r += delta_r;
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

		mRenderComponent->RenderScene();
	}

	mRenderComponent->Cleanup();
}