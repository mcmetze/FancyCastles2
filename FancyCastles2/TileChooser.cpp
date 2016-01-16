#include "TileChooser.h"

#include "BoardRenderer.h"
#include "Commands.h"
#include "PlayerController.h"

TileChooser::TileChooser(int numPlayers, int numTilesToChoose, BoardRenderer& renderComponent)
	:mNumPlayers(numPlayers), mNumTilesToChoose(numTilesToChoose), mRenderer(renderComponent), mCurPlayerChoosing(1)
{

}

TileChooser::~TileChooser()
{
}

void
TileChooser::ChooseTiles()
{
	while (mNumTilesToChoose >=0)
	{
		mRenderer.RenderScene();
	}
}

void
TileChooser::AutoChooseTiles()
{
	int playerID = 1;
	for (int i = 0; i < mNumTilesToChoose; ++i)
	{
		mTileIDsForEachPlayer[i] = playerID;
		playerID = (playerID % mNumPlayers) + 1;
	}
}

void
TileChooser::AssignTilesToPlayers(PlayerController& pc)
{
	for (auto tileMapping : mTileIDsForEachPlayer)
	{
		pc.AddTileToPlayer(tileMapping.first, tileMapping.second);
	}
}

void
TileChooser::OnNotify(Command* cmd)
{
	auto pickCmd = dynamic_cast<PickSelectionCommand*>(cmd);
	if (pickCmd)
	{
		const auto selectedTile = SelectTileFromMouse();
		ChooseTileIfAvailable(selectedTile);
		return;
	}

	auto changePlayerCmd = dynamic_cast<ChangePlayerCommand*>(cmd);
	if (changePlayerCmd)
	{
		mCurPlayerChoosing = changePlayerCmd->GetPlayerID();
		return;
	}
}

void
TileChooser::OnNotify(TimerResultPtr result)
{
}

int
TileChooser::SelectTileFromMouse() const
{
	return mRenderer.DoPick();
}

void
TileChooser::ChooseTileIfAvailable(int tileID)
{
	if (mTileIDsForEachPlayer.count(tileID) > 0)
		return;

	mTileIDsForEachPlayer[tileID] = mCurPlayerChoosing;
	mNumTilesToChoose--;
}