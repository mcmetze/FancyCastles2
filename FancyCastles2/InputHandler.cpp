#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow* window)
	: mWindow(window)
	, mLastKeyPress(NO_KEY_PRESS)
	, mLastMouseClick(NO_MOUSE_CLICK)
	, mLeftCommand(new MoveSelectionCommand(0, -1))
	, mRightCommand(new MoveSelectionCommand(0, 1))
	, mUpCommand(new MoveSelectionCommand(-1, 0))
	, mDownCommand(new MoveSelectionCommand(1, 0))
	, mPickCommand(new PickSelectionCommand())
	, mNullCommand(new NullCommand())
	, mExitCommand(new ExitGameCommand())
	
{
}

InputHandler::~InputHandler()
{
	delete mLeftCommand;
	delete mRightCommand;
	delete mUpCommand;
	delete mDownCommand;
	delete mPickCommand;
	delete mNullCommand;
	delete mExitCommand;
}

Command*
InputHandler::HandleKeyPress()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(mWindow, GL_TRUE);
		return mExitCommand;
	}



	if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		mLastKeyPress = InputHandler::LEFT_KEY;
	}
	if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_RELEASE && mLastKeyPress == InputHandler::LEFT_KEY)
	{
		mLastKeyPress = InputHandler::NO_KEY_PRESS;
		return mLeftCommand;
	}

	if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		mLastKeyPress = InputHandler::RIGHT_KEY;
	}
	if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_RELEASE && mLastKeyPress == InputHandler::RIGHT_KEY)
	{
		mLastKeyPress = InputHandler::NO_KEY_PRESS;
		return mRightCommand;
	}

	if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mLastKeyPress = InputHandler::UP_KEY;
	}
	if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_RELEASE && mLastKeyPress == InputHandler::UP_KEY)
	{
		mLastKeyPress = InputHandler::NO_KEY_PRESS;
		return mUpCommand;
	}

	if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mLastKeyPress = InputHandler::DOWN_KEY;
	}
	if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_RELEASE && mLastKeyPress == InputHandler::DOWN_KEY)
	{
		mLastKeyPress = InputHandler::NO_KEY_PRESS;
		return mDownCommand;
	}

	return mNullCommand;
}

Command*
InputHandler::HandleMouseClick()
{
	// flag the click for release
	if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		mLastMouseClick = InputHandler::LEFT_MOUSE;
	}

	// released from a left mouse click, flag this to do a pick
	if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mLastMouseClick == InputHandler::LEFT_MOUSE)
	{
		mLastMouseClick = InputHandler::NO_MOUSE_CLICK;
		return mPickCommand;
	}

	return mNullCommand;
}



void
MoveSelectionCommand::Execute(GameManager* gm)
{
	gm->UpdateSelectedTilePosition(mDx, mDy);
}

void
PickSelectionCommand::Execute(GameManager* gm)
{
	gm->DoPick();
}