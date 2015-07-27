#pragma once

#include <memory>
#include <unordered_map>

class Command;
class GameManager;
class GLFWwindow;

class InputHandler
{
public:
	InputHandler(GLFWwindow* window);
	~InputHandler();

	void HandleMouseClick(int buton);
	void HandleKeyPress(int key);

	void SetGameManager(GameManager* gm);
	
private:
	GameManager* mGameManager;

	std::unordered_map<int, std::unique_ptr<Command> > mKeyboardInputMap;
	std::unordered_map<int, std::unique_ptr<Command> > mMouseInputMap;


};