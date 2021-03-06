#include <GLFW/glfw3.h>

#include "InputHandler.h"

#include "Commands.h"

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
	mKeyboardInputMap[GLFW_KEY_SPACE] = std::make_unique<HarvestCommand>();
	mKeyboardInputMap[GLFW_KEY_B] = std::make_unique<BuildCommand>();
	mKeyboardInputMap[GLFW_KEY_ESCAPE] = std::make_unique<ExitGameCommand>();

	//TODO- there must be a better way
	mKeyboardInputMap[GLFW_KEY_1] = std::make_unique<ChangePlayerCommand>(1);
	mKeyboardInputMap[GLFW_KEY_2] = std::make_unique<ChangePlayerCommand>(2);
	mKeyboardInputMap[GLFW_KEY_3] = std::make_unique<ChangePlayerCommand>(3);
	mKeyboardInputMap[GLFW_KEY_4] = std::make_unique<ChangePlayerCommand>(4);
	mKeyboardInputMap[GLFW_KEY_5] = std::make_unique<ChangePlayerCommand>(5);


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
		Notify(command->second.get());
}

void 
InputHandler::HandleMouseClick(int button)
{
	const auto command = mMouseInputMap.find(button);
	if (command != mMouseInputMap.end())
		Notify(command->second.get());
}