#include "InputHandler.h"

static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (action == GLFW_RELEASE)
	{
		auto inputHandler = reinterpret_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->HandleKeyPress(key);
	}
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		auto inputHandler = reinterpret_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->HandleMouseClick(button);
	}
}

InputHandler::InputHandler(GLFWwindow* window)
{
	glfwSetKeyCallback(window, KeyboardCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	mKeyboardInputMap[GLFW_KEY_LEFT] = std::make_unique<MoveSelectionCommand>(0, -1);
	mKeyboardInputMap[GLFW_KEY_RIGHT] = std::make_unique<MoveSelectionCommand>(0, 1);
	mKeyboardInputMap[GLFW_KEY_UP] = std::make_unique<MoveSelectionCommand>(-1, 0);
	mKeyboardInputMap[GLFW_KEY_DOWN] = std::make_unique<MoveSelectionCommand>(1, 0);
	mKeyboardInputMap[GLFW_KEY_SPACE] = std::make_unique<HarvestRawResourceCommand>();
	mKeyboardInputMap[GLFW_KEY_ESCAPE] = std::make_unique<ExitGameCommand>();

	mMouseInputMap[GLFW_MOUSE_BUTTON_LEFT] = std::make_unique<PickSelectionCommand>();
}

InputHandler::~InputHandler()
{
}

void
InputHandler::HandleKeyPress(int key)
{
	const auto command = mKeyboardInputMap.find(key);
	if (command != mKeyboardInputMap.end())
		command->second->Execute(mGameManager);
}

void 
InputHandler::HandleMouseClick(int button)
{
	const auto command = mMouseInputMap.find(button);
	if (command != mMouseInputMap.end())
		command->second->Execute(mGameManager);
}


void
MoveSelectionCommand::Execute(GameManager* gm)
{
	gm->MoveTileSelection(mDx, mDy);
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