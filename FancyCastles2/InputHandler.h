#pragma once

#include <memory>
#include <unordered_map>

#include "Observer.h"

class Command;

struct GLFWwindow;

class InputHandler : public Observable
{
public:
	InputHandler(GLFWwindow* window);
	~InputHandler();

	void HandleMouseClick(int buton);
	void HandleKeyPress(int key);
	
private:
	std::unordered_map<int, std::unique_ptr<Command> > mKeyboardInputMap;
	std::unordered_map<int, std::unique_ptr<Command> > mMouseInputMap;

};