#ifndef METZE_INPUT_H
#define METZE_INPUT_H

#include <GLFW/glfw3.h>

#include "GameManager.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void Execute(GameManager* gm) = 0;
};

class MoveSelectionCommand : public Command
{
public:
	MoveSelectionCommand(int dx, int dy) : mDx(dx), mDy(dy) { }

	virtual void Execute(GameManager* gm);

private:
	int mDx;
	int mDy;
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

class NullCommand : public Command
{
public:
	virtual void Execute(GameManager* gm){ }
};

class ExitGameCommand : public Command
{
public:
	virtual void Execute(GameManager* gm) { gm->ExitGame(); }
};




class InputHandler
{
public:
	InputHandler(GLFWwindow* window);
	~InputHandler();

	void HandleMouseClick(int buton);
	void HandleKeyPress(int key);

	void SetGameManager(GameManager* gm) { mGameManager = gm; }
	
private:
	GameManager* mGameManager;

	std::unordered_map<int, std::unique_ptr<Command> > mKeyboardInputMap;
	std::unordered_map<int, std::unique_ptr<Command> > mMouseInputMap;


};

#endif