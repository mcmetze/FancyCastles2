#include "TileTraits.h"

#include "Board.h"

#include <cassert>

TileSelection::TileSelection() :
  mIsValid(false)
, mTileIndex(INT_MAX)
, mTilePos(INT_MAX, INT_MAX)
, mBoardInstance(nullptr)
{
}

void
TileSelection::SetBoard(const Board* board)
{
	mBoardInstance = board;
}

void
TileSelection::Update(const int& tileIndex)
{
	assert(mBoardInstance);

	mIsValid = (tileIndex >= 0 && tileIndex < mBoardInstance->GetNumTiles());

	if (!mIsValid)
		return;

	mTileIndex = tileIndex;
	mTilePos = mBoardInstance->GetTileCoord(tileIndex);
}

void
TileSelection::Update(const AxialCoord& tilePos)
{
	assert(mBoardInstance);

	mIsValid = mBoardInstance->IsPositionValid(tilePos);

	if (!mIsValid)
		return;

	mTilePos = tilePos;
	mTileIndex = mBoardInstance->GetTileIndex(tilePos);
}

void
TileSelection::Clear()
{
	mTileIndex = INT_MAX;
	mTilePos = { INT_MAX, INT_MAX };
	mIsValid = false;
}

bool
TileSelection::IsValid() const
{
	return mIsValid;
}

int
TileSelection::GetSelectionIndex() const
{
	return mTileIndex;
}

AxialCoord
TileSelection::GetSelectionPos() const
{
	return mTilePos;
}