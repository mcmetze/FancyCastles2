#define GLEW_STATIC
#include <GL/glew.h>

#include <vld.h>

#include "Board.h"
#include "BoardController.h"
#include "BoardRenderer.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "Player.h"
#include "PlayerController.h"
#include "TileChooser.h"
#include "Timer.h"

const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 768;
const float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;
const int NUM_PLAYERS = 6;

using PlayerList = std::vector < std::unique_ptr<Player> > ;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

GLFWwindow*
CreateRenderWindow()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Fancy Castles", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}

	return window;
}

std::unique_ptr<Board>
CreateGameBoard()
{
	auto gameBoard = std::make_unique<Board>();
	gameBoard->MakeBoard(NUM_PLAYERS);

	return gameBoard;
}

std::unique_ptr<BoardRenderer>
CreateGameRenderer(const Board& board, GLFWwindow* window)
{
	auto renderComponent = std::make_unique<BoardRenderer>(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	renderComponent->SetupTiles(board);
	renderComponent->Init();

	return renderComponent;
}

std::unique_ptr<InputHandler>
CreateInputHandler(GLFWwindow* renderWindow)
{
	auto inputComponent = std::make_unique<InputHandler>(renderWindow);
	glfwSetWindowUserPointer(renderWindow, inputComponent.get());

	return inputComponent;
}

std::shared_ptr<PlayerController>
CreatePlayerController(int numPlayers)
{
	const int numBills = 5;
	PlayerList players;
	for (int playerId = 0; playerId < numPlayers; ++playerId)
	{
		auto timer = std::make_unique<TimerObject>(playerId);
		players.push_back(std::make_unique<Player>(playerId, std::move(timer), numBills));
	}

	return std::make_shared<PlayerController>(players);
}

int main(void)
{
	auto renderWindow = CreateRenderWindow();
	auto inputComponent = CreateInputHandler(renderWindow);

	auto gameBoard = CreateGameBoard();
	const auto numTiles = gameBoard->GetNumTiles();

	auto renderComponent = CreateGameRenderer(*gameBoard, renderWindow);
	auto boardController = std::make_unique<BoardController>(std::move(gameBoard));
	auto playerController = CreatePlayerController(NUM_PLAYERS);

	{
		auto chooser = std::make_shared<TileChooser>(NUM_PLAYERS, 10, *renderComponent);
		inputComponent->AddObserver(chooser);
		chooser->ChooseTiles();
		//chooser->AutoChooseTiles();
		chooser->AssignTilesToPlayers(*playerController);
	}

	{
		auto manager = std::make_shared<GameManager>(std::move(renderComponent), std::move(boardController), playerController);
		inputComponent->AddObserver(manager);
		playerController->ObserveTimers(manager);

		manager->StartGame();
	}

	glfwDestroyWindow(renderWindow);
	glfwTerminate();
}

