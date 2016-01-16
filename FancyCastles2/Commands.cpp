#include "Commands.h"


MoveSelectionCommand::MoveSelectionCommand(const int& dr, const int& dq)
	: mOffset({ dq, dr })
{
}

void
MoveSelectionCommand::Execute()
{
	//gm->MoveTileSelection(mOffset);
}

AxialCoord
MoveSelectionCommand::GetOffset() const
{
	return mOffset;
}

void
PickSelectionCommand::Execute()
{
	//gm->SelectTileFromMouse();
}

void
HarvestCommand::Execute()
{
	//gm->HarvestResource();
}

void
BuildCommand::Execute()
{
	//gm->Build();
}

void
ExitGameCommand::Execute()
{
	//gm->ExitGame();
}

ChangePlayerCommand::ChangePlayerCommand(int playerID)
	: mPlayerID(playerID)
{
}

int
ChangePlayerCommand::GetPlayerID() const
{
	return mPlayerID;
}

void
ChangePlayerCommand::Execute()
{

}