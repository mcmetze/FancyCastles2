#pragma once

#include "TileTraits.h"


class Command
{
public:
	virtual ~Command() {}
	virtual void Execute() = 0;
};

class MoveSelectionCommand : public Command
{
public:
	MoveSelectionCommand(const int& dr, const int& dq);

	void Execute() override;

	AxialCoord GetOffset() const;

private:
	AxialCoord mOffset;
};

class HarvestCommand : public Command
{
public:
	void Execute() override;
};

class PickSelectionCommand : public Command
{
public:
	void Execute() override;
};

class BuildCommand : public Command
{
public:
	void Execute() override;
};

class NullCommand : public Command
{
public:
	void Execute() override { }
};

class ExitGameCommand : public Command
{
public:
	void Execute() override;
};

class ChangePlayerCommand : public Command
{
public:
	ChangePlayerCommand(int playerID);

	int GetPlayerID() const;

	void Execute() override;

private:
	int mPlayerID;
};