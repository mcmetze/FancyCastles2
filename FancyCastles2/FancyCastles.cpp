#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <glm.hpp>

#include <vld.h>

#include "Board.h"
#include "HexGeometry.h"

static const float WINDOW_WIDTH = 1366;
static const float WINDOW_HEIGHT = 768;
static const float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

// Shader sources
const GLchar* vertexSource =
"#version 130 \n"
"in vec2 position;"
"in vec3 color;"
"out vec3 Color;"
"void main() {"
"   Color = color;"
"   gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 0.0, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 130 \n"
"in vec3 Color;"
"out vec4 outColor;"
"void main() {"
"   outColor = vec4(Color, 1.0);"
"}";


class FancyCastlesView
{

public:

	FancyCastlesView(const int& numPlayers) :
		  mVertexShader(0)
		, mFragmentShader(0)
		, mShaderProgram(0)
		, mHexSize(0)
		, mHexHeight(0)
		, mHexWidth(0)
		, mTilesHeight(0)
		, mNumTiles(0)
		, mNumPlayers(numPlayers)
		, mGameBoard(nullptr)
		, mColorBufferPtr(nullptr)
	{
		CreateGameBoard(numPlayers);
	}

	void CreateGameBoard(const int& numPlayers)
	{
		mGameBoard = std::make_unique<Board>();
		mGameBoard->MakeBoard(numPlayers);
		mNumTiles = mGameBoard->GetNumTiles();
	}

	void GetVertexColorFromType(GLfloat& r, GLfloat& g, GLfloat& b, const ResourceType& tileType)
	{
		//placeholder method to verify

		r = g = b = 0.0;
		switch (tileType)
		{
		case WATER:
			b = 1.f;
			break;
		case GRASS:
			g = 1.f;
			break;
		case WHEAT:
			r = 1.f;
			g = 1.f;
			break;
		case TREE:
			r = 0.6f;
			g = 0.29f;
			break;
		case ORE:
			r = 0.47f;
			g = 0.47f;
			b = 0.47f;
			break;
		default:
			break;
		}
	}

	void GetUniqueTileColor(GLfloat& r, GLfloat& g, GLfloat& b, const int& index)
	{
		//convert the tile's index within the board to an RGB color
		unsigned char redVal  =	 (index & 0x000000FF);
		unsigned char blueVal =  (index & 0x0000FF00);
		unsigned char greenVal = (index & 0x00FF0000);

		r = redVal / 255.f;
		g = (greenVal >> 8) / 255.f;
		b = (blueVal >> 16) / 255.f;

		unsigned int rgbVal = redVal | (greenVal << 8) | (blueVal << 16);
		mColorToTileMap[rgbVal] = index;
	}

	void SetupHexVerts()
	{
		//create a hex shape to reuse on the board
		mHexSize = 1;
		HexGeometry hexGeo(mHexSize);
		hexGeo.CreateHex();
		auto hexVerts = hexGeo.GetHexVerts();

		//gives a little space between tiles
		const float boarder = 0.01f;
		mHexHeight = hexGeo.GetHeight() + boarder;
		mHexWidth = hexGeo.GetWidth() + boarder;

		//used for making the board fit on the screen
		mTilesHeight = mHexSize * (mNumTiles / mNumPlayers+1);

		for (auto tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//get a tile from the board and convert the axial coordinate to cartesian
			auto position = mGameBoard->GetTileCoord(tileIndex);
			GLfloat x = mHexWidth * position.r + mHexWidth / 2.f * position.q;
			GLfloat y = -0.75f*mHexHeight * position.q;
			
			//translate each vertex of the hex and add it to the buffer
			for (auto curVert : hexVerts)
			{
				mVertexInfo.push_back(x + curVert.x);
				mVertexInfo.push_back(y + curVert.y);
			}
		}

		//naive trianglulation of each tile 
		//used for the element/index buffer
		for (auto tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			GLuint firstTriVertIndex = tileIndex * 6;
			GLuint curVertIndex = firstTriVertIndex;
			for (auto curTriangle = 0; curTriangle < 4; ++curTriangle)
			{
				mVertIndices.push_back(firstTriVertIndex);
				mVertIndices.push_back(curVertIndex + 1);
				mVertIndices.push_back(curVertIndex + 2);
				curVertIndex++;
			}
		}

	}

	void SetupTileColors()
	{
		GLfloat r, g, b;
		for (auto tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//TODO: use texture instead of flat color
			auto tileType = mGameBoard->GetTileType(tileIndex);
			GetVertexColorFromType(r, g, b, tileType);
			for (auto i = 0; i < 6; ++i)
			{
				mColorInfo.push_back(r);
				mColorInfo.push_back(g);
				mColorInfo.push_back(b);
			}

			//used for picking later
			GetUniqueTileColor(r, g, b, tileIndex);
			for (auto i = 0; i < 6; ++i)
			{
				mColorIDs.push_back(r);
				mColorIDs.push_back(g);
				mColorIDs.push_back(b);
			}
		}
	}

	void SetupShaders()
	{
		// Create and compile the vertex shader
		mVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(mVertexShader, 1, &vertexSource, NULL);
		glCompileShader(mVertexShader);

		// Create and compile the fragment shader
		mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(mFragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(mFragmentShader);

		// Link the vertex and fragment shader into a shader program
		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, mVertexShader);
		glAttachShader(mShaderProgram, mFragmentShader);
		glBindFragDataLocation(mShaderProgram, 0, "outColor");
		glLinkProgram(mShaderProgram);

		//check errors
		GLint success = 0;
		GLchar ErrorLog[1024] = { 0 };
		glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
		if (success == 0) {
			glGetProgramInfoLog(mShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
			//exit(1);
		}

		glValidateProgram(mShaderProgram);
		glGetProgramiv(mShaderProgram, GL_VALIDATE_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(mShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
			//exit(1);
		}
		glUseProgram(mShaderProgram);
	}

	void RenderLoop(GLFWwindow* window)
	{
		SetupHexVerts();
		SetupTileColors();

		// Create a Vertex Buffer Object and copy the vertex data to it
		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mVertexInfo.size(), mVertexInfo.data(), GL_STATIC_DRAW);

		// Create a separate buffer for the color information
		GLuint colorBuffer;
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mColorInfo.size(), mColorInfo.data(), GL_DYNAMIC_DRAW);

		// Create an element array
		GLuint elementBuffer;
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*mVertIndices.size(), mVertIndices.data(), GL_STATIC_DRAW);

		SetupShaders();

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(mShaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		GLint colAttrib = glGetAttribLocation(mShaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//just use ortho for now, adjust screen based on board size and aspect
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-(ASPECT_RATIO*mTilesHeight), ASPECT_RATIO*mTilesHeight, -mTilesHeight, mTilesHeight, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);

		bool leftMousePressed = false;
		double mouseX = 0.0;
		double mouseY = 0.0;
		while (!glfwWindowShouldClose(window))
		{
			// flag the click for release
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				leftMousePressed = true;
			}
			
			// released from a left mouse click, do the first render pass for the pick operation
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && leftMousePressed)
			{
				leftMousePressed = false;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				mouseY = WINDOW_HEIGHT - mouseY;

				//clear to white because black is generated in the unique tile colors for index 0
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

				//use the color buffer with the unique tile colors
				mColorBufferPtr = &mColorIDs;
				glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(colAttrib);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
				glDrawElements(GL_TRIANGLES, mVertIndices.size(), GL_UNSIGNED_INT, 0);

				glFlush();
				glFinish();

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				//read the pixel at the mouse position and look up the tile that maps to this color in the color/tile map
				unsigned char data[4];
				glReadPixels(static_cast<GLint>(floor(mouseX)), static_cast<GLint>(floor(mouseY)), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
				unsigned int rgbVal = data[0] | (data[1] << 8) | (data[2] << 16);

				if (mColorToTileMap.find(rgbVal) == mColorToTileMap.end())
				{
					printf("DID NOT FIND TILE\n");
					printf("%#04x, %#04x, %#04x\n", data[0], data[1], data[2]);
				}
				else
				{
					int tileID = mColorToTileMap[rgbVal];
					printf("found tile %i\n", tileID);
					printf("position {%i, %i}\n", mGameBoard->GetTileCoord(tileID).r, mGameBoard->GetTileCoord(tileID).q);
					switch (mGameBoard->GetTileType(tileID))
					{
					case WATER:
						printf("water\n");
						break;
					case GRASS:
						printf("grass\n");
						break;
					case WHEAT:
						printf("wheat\n");
						break;
					case ORE:
						printf("ore\n");
						break;
					case TREE:
						printf("tree\n");
						break;
					default:
						break;
					}
				}
			}

			//second render pass with the real colors
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
			mColorBufferPtr = &mColorInfo;
			glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(colAttrib);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
			glDrawElements(GL_TRIANGLES, mVertIndices.size(), GL_UNSIGNED_INT, 0);


			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteProgram(mShaderProgram);
		glDeleteShader(mFragmentShader);
		glDeleteShader(mVertexShader);

		glDeleteBuffers(1, &elementBuffer);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &colorBuffer);
	}

private:

	GLuint mVertexShader;
	GLuint mFragmentShader;
	GLuint mShaderProgram;

	int mHexSize;
	float mHexHeight;
	float mHexWidth;

	int mNumPlayers;
	int mTilesHeight;
	size_t mNumTiles;
	std::unique_ptr<Board> mGameBoard;

	std::map<unsigned int, int> mColorToTileMap;

	std::vector<GLfloat>* mColorBufferPtr;
	std::vector<GLfloat> mVertexInfo;
	std::vector<GLfloat> mColorInfo;
	std::vector<GLfloat> mColorIDs;
	std::vector<GLuint> mVertIndices;
};







static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
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

	glfwSetKeyCallback(window, key_callback);

	{
		FancyCastlesView gameInstance(6);
		gameInstance.RenderLoop(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

