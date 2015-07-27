#include "Commands.h"

#include "GameManager.h"

MoveSelectionCommand::MoveSelectionCommand(const int& dr, const int& dq)
	: mOffset({ dq, dr })
{
}

void
MoveSelectionCommand::Execute(GameManager* gm)
{
	gm->MoveTileSelection(mOffset);
}

void
PickSelectionCommand::Execute(GameManager* gm)
{
	gm->SelectTileFromMouse();
}

void
HarvestRawResourceCommand::Execute(GameManager* gm)
{
	gm->HarvestResource();
}

void
BuildCommand::Execute(GameManager* gm)
{
	gm->Build();
}

void
ExitGameCommand::Execute(GameManager* gm)
{
	gm->ExitGame();
}