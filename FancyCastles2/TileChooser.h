#pragma once

#include <memory>
#include <unordered_map>

#include "Observer.h"

class BoardRenderer;
class PlayerController;

class TileChooser : public Observer
{
public:
	TileChooser() = delete;
	TileChooser(const TileChooser&) = delete;
	TileChooser& operator=(const TileChooser& rhs) = delete;

	TileChooser(int numPlayers, int numTilesToChoose, BoardRenderer& renderComponent);
	~TileChooser();

	void ChooseTiles();
	void AutoChooseTiles();
	void AssignTilesToPlayers(PlayerController& pc);

	void OnNotify(Command* cmd) override;
	void OnNotify(TimerResultPtr result) override;

private:
	int SelectTileFromMouse() const;
	void ChooseTileIfAvailable(int tileID);

	int mNumPlayers;
	int mNumTilesToChoose;
	int mCurPlayerChoosing;
	BoardRenderer& mRenderer;
	std::unordered_map<int, int> mTileIDsForEachPlayer;
};