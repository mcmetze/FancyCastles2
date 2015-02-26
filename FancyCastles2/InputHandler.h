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
	enum KeyPress { NO_KEY_PRESS, LEFT_KEY, RIGHT_KEY, UP_KEY, DOWN_KEY };
	enum MouseClick { NO_MOUSE_CLICK, LEFT_MOUSE, MIDDLE_MOUSE, RIGHT_MOUSE };

	InputHandler(GLFWwindow* window);
	~InputHandler();

	Command* HandleMouseClick();
	Command* HandleKeyPress();

private:
	Command* mLeftCommand;
	Command* mRightCommand;
	Command* mUpCommand;
	Command* mDownCommand;

	Command* mPickCommand;

	Command* mNullCommand;
	Command* mExitCommand;

	KeyPress mLastKeyPress;
	MouseClick mLastMouseClick;

	GLFWwindow* mWindow;
};

#endif