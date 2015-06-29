#define GLEW_STATIC
#include <GL/glew.h>

#include <vld.h>

#include "GameManager.h"
#include "InputHandler.h"

static const float WINDOW_WIDTH = 1366;
static const float WINDOW_HEIGHT = 768;
static const float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main(void)
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

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

	{
		const int NUM_PLAYERS = 6;
		std::unique_ptr<BoardRenderer> gameRenderer = std::make_unique<BoardRenderer>(window, WINDOW_WIDTH, WINDOW_HEIGHT);
		std::unique_ptr<InputHandler> inputHandler = std::make_unique<InputHandler>(window);
		glfwSetWindowUserPointer(window, inputHandler.get() );

		GameManager game(NUM_PLAYERS, std::move(gameRenderer), std::move(inputHandler));
		//game.SetupPlayers();
		game.AssignPlayers();
		game.GameLoop();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

