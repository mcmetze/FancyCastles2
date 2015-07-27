#pragma once

#include "TileTraits.h"

class GameManager;

class Command
{
public:
	virtual ~Command() {}
	virtual void Execute(GameManager* gm) = 0;
};

class MoveSelectionCommand : public Command
{
public:
	MoveSelectionCommand(const int& dr, const int& dq);

	virtual void Execute(GameManager* gm);

private:
	AxialCoord mOffset;
};

class HarvestRawResourceCommand : public Command
{
public:
	virtual void Execute(GameManager* gm);
};

class PickSelectionCommand : public Command
{
public:
	virtual void Execute(GameManager* gm);
};

class BuildCommand : public Command
{
public:
	virtual void Execute(GameManager* gm);
};

class NullCommand : public Command
{
public:
	virtual void Execute(GameManager* gm){ }
};

class ExitGameCommand : public Command
{
public:
	virtual void Execute(GameManager* gm);
};