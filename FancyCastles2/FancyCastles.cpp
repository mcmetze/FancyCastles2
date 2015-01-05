#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL.h"

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <glm.hpp>

#include <vld.h>

#include "Board.h"
#include "HexGeometry.h"

static const float WINDOW_WIDTH = 1366;
static const float WINDOW_HEIGHT = 768;
static const float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

class FancyCastlesView
{

public:

	FancyCastlesView(const int& numPlayers) :
		mVertexShader(0)
		, mFragmentShader(0)
		, mShaderProgram(0)
		, mNumTiles(0)
		, mNumPlayers(numPlayers)
		, mGameBoard(nullptr)
		, mColorBufferPtr(nullptr)
		, mLastKeyPress(NO_KEY_PRESS)
		, mLastMouseClick(NO_MOUSE_CLICK)
		, mDoPick(false)
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
		unsigned char redVal = (index & 0x000000FF);
		unsigned char blueVal = (index & 0x0000FF00);
		unsigned char greenVal = (index & 0x00FF0000);

		r = redVal / 255.f;
		g = (greenVal >> 8) / 255.f;
		b = (blueVal >> 16) / 255.f;

		unsigned int rgbVal = redVal | (greenVal << 8) | (blueVal << 16);
		mColorToTileMap[rgbVal] = index;
	}

	void PrintTileInfo(int tileID)
	{
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

	void SetupTileColors()
	{
		GLfloat r, g, b;
		for (size_t tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//TODO: use textures
			auto tileType = mGameBoard->GetTileType(tileIndex);
			GetVertexColorFromType(r, g, b, tileType);
			mColorInfo.push_back(glm::vec3(r, g, b));

			//used for picking later
			GetUniqueTileColor(r, g, b, tileIndex);
			mColorIDs.push_back(glm::vec3(r, g, b));
		}
	}

	void GetCartesianFromAxial(GLfloat& x, GLfloat& y, const AxialCoord& position)
	{
		//gives a little space between tiles
		static float boarder = 0.01f;
		static int hexSize = 1;
		static float hexHeight = 2.0f * hexSize + boarder;
		static float hexWidth = 0.5f * sqrt(3.0f) * hexHeight + boarder;

		x = hexWidth * position.r + hexWidth / 2.f * position.q;
	    y = -0.75f*hexHeight * position.q;
	}

	void SetupHexVerts()
	{
		for (size_t tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//get a tile from the board and convert the axial coordinate to cartesian
			GLfloat x, y; 
			GetCartesianFromAxial(x, y, mGameBoard->GetTileCoord(tileIndex));

			mVertexInfo.push_back(glm::vec2(x, y));
		}
	}

	unsigned int DoPick(GLFWwindow* window)
	{
		double mouseX = 0.0;
		double mouseY = 0.0;

		glfwGetCursorPos(window, &mouseX, &mouseY);
		mouseY = WINDOW_HEIGHT - mouseY;

		//clear to white because black is generated in the unique tile colors for index 0
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//use the color buffer with the unique tile colors
		mColorBufferPtr = &mColorIDs;
		RenderPass();

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//read the pixel at the mouse position and convert it to the color ID we used to make the board with unique tile colors
		unsigned char data[4];
		glReadPixels(static_cast<GLint>(mouseX), static_cast<GLint>(mouseY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return (data[0] | (data[1] << 8) | (data[2] << 16));
	}

	GLuint LoadTexture(std::string imagePath)
	{
		GLuint texture = SOIL_load_OGL_texture(imagePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return texture;
	}

	void BindTexture(GLenum TextureUnit, GLuint tex)
	{
		glActiveTexture(TextureUnit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	bool CheckShader(const GLuint& shader)
	{
		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			for (auto c : errorLog)
				std::cout << c;
			glDeleteShader(shader); // Don't leak the shader.
			return false;
		}

		return true;
	}

	std::string LoadShaderFile(const char* filename)
	{
		std::ifstream in(filename);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}

	void SetupShaders()
	{
		auto vertexSource = LoadShaderFile("vert.glsl");
		auto geometrySource = LoadShaderFile("geo.glsl");
		auto fragmentSource = LoadShaderFile("frag.glsl");
		const GLchar* sources[] = { vertexSource.c_str(), geometrySource.c_str(), fragmentSource.c_str() };

		// Create and compile the vertex shader
		mVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(mVertexShader, 1, &sources[0], NULL);
		glCompileShader(mVertexShader);
		CheckShader(mVertexShader);

		GLuint mGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(mGeometryShader, 1, &sources[1], NULL);
		glCompileShader(mGeometryShader);
		CheckShader(mGeometryShader);

		// Create and compile the fragment shader
		mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(mFragmentShader, 1, &sources[2], NULL);
		glCompileShader(mFragmentShader);
		CheckShader(mFragmentShader);

		// Link the vertex and fragment shader into a shader program
		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, mVertexShader);
		glAttachShader(mShaderProgram, mGeometryShader);
		glAttachShader(mShaderProgram, mFragmentShader);

		glBindFragDataLocation(mShaderProgram, 0, "outColor");
		glLinkProgram(mShaderProgram);

		//check errors
		GLint success = 0;
		GLchar ErrorLog[1024] = { 0 };
		glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
		if (success == 0) 
		{
			glGetProgramInfoLog(mShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
			exit(EXIT_FAILURE);
		}

		glValidateProgram(mShaderProgram);
		glGetProgramiv(mShaderProgram, GL_VALIDATE_STATUS, &success);
		if (!success) 
		{
			glGetProgramInfoLog(mShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
			exit(EXIT_FAILURE);
		}
		glUseProgram(mShaderProgram);
	}

	void SetupBuffers()
	{
		// Create a Vertex Buffer Object and copy the vertex data to it
		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*mVertexInfo.size(), mVertexInfo.data(), GL_STATIC_DRAW);

		// Create a separate buffer for the color information
		glGenBuffers(1, &mColorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mColorInfo.size(), mColorInfo.data(), GL_DYNAMIC_DRAW);
		mColorBufferPtr = &mColorInfo;
	}

	void SetupAttributes()
	{
		// Specify the layout of the vertex data
		mPosAttrib = glGetAttribLocation(mShaderProgram, "position");
		glEnableVertexAttribArray(mPosAttrib);
		glVertexAttribPointer(mPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		GLint colAttrib = glGetAttribLocation(mShaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void HandleKeyPress(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			mLastKeyPress = LEFT_KEY;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE && mLastKeyPress == LEFT_KEY)
		{
			mLastKeyPress = NO_KEY_PRESS;
			mSelectedTilePos.r--;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			mLastKeyPress = RIGHT_KEY;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE && mLastKeyPress == RIGHT_KEY)
		{
			mLastKeyPress = NO_KEY_PRESS;
			mSelectedTilePos.r++;
		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			mLastKeyPress = UP_KEY;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && mLastKeyPress == UP_KEY)
		{
			mLastKeyPress = NO_KEY_PRESS;
			mSelectedTilePos.q--;
		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			mLastKeyPress = DOWN_KEY;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && mLastKeyPress == DOWN_KEY)
		{
			mLastKeyPress = NO_KEY_PRESS;
			mSelectedTilePos.q++;
		}
	}

	void HandleMouseClick(GLFWwindow* window)
	{
		// flag the click for release
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			mLastMouseClick = LEFT_MOUSE;
		}

		// released from a left mouse click, flag this to do a pick
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mLastMouseClick == LEFT_MOUSE)
		{
			mLastMouseClick = NO_MOUSE_CLICK;
			mDoPick = true;
		}
	}

	void RenderPass()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);

		glDrawArrays(GL_POINTS, 0, mVertexInfo.size());
	}

	void Cleanup()
	{
		glDeleteProgram(mShaderProgram);
		glDeleteShader(mGeometryShader);
		glDeleteShader(mFragmentShader);
		glDeleteShader(mVertexShader);

		glDeleteBuffers(1, &mVertexBuffer);
		glDeleteBuffers(1, &mColorBuffer);
	}

	void RenderLoop(GLFWwindow* window)
	{
		SetupHexVerts();
		SetupTileColors();

		SetupBuffers();
		SetupShaders();
		SetupAttributes();

		BindTexture(GL_TEXTURE0, LoadTexture("TileOutline.png"));
		glUniform1i(glGetUniformLocation(mShaderProgram, "tileOutlineTex"), 0);
		mSelectedPointUniform = glGetUniformLocation(mShaderProgram, "selectedPoint");
		mSelectedTilePos = AxialCoord(mNumTiles, mNumTiles);

		while (!glfwWindowShouldClose(window))
		{
			HandleKeyPress(window);
			HandleMouseClick(window);

			if (mDoPick)
			{
				//do the first render pass with the unique tile colors and check if the user clicked a tile
				unsigned int rgbVal = DoPick(window);

				//lookup which tile was clicked (if any)
				if (mColorToTileMap.find(rgbVal) == mColorToTileMap.end())
				{
					printf("DID NOT FIND TILE\n");
					mSelectedTilePos = AxialCoord(mNumTiles, mNumTiles);
				}
				else
				{
					int tileID = mColorToTileMap[rgbVal];
					printf("found tile %i\n", tileID);
					PrintTileInfo(tileID);
					mSelectedTilePos = mGameBoard->GetTileCoord(tileID);
				}

				mDoPick = false;
			}

			//update the selected tile/outline texture
			GLfloat selectedPos[] = { 0.f, 0.f };
			GetCartesianFromAxial(selectedPos[0], selectedPos[1], mSelectedTilePos);
			glUniform2fv(mSelectedPointUniform, 1, selectedPos);

			glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
			glVertexAttribPointer(mPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

			//second render pass with the real colors
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			mColorBufferPtr = &mColorInfo;
			RenderPass();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		Cleanup();
	}

private:

	GLuint mVertexShader;
	GLuint mGeometryShader;
	GLuint mFragmentShader;
	GLuint mShaderProgram;

	GLint mSelectedPointUniform;
	GLint mPosAttrib;

	GLuint mColorBuffer;
	GLuint mVertexBuffer;

	std::vector<glm::vec3>* mColorBufferPtr;
	std::vector<glm::vec2> mVertexInfo;
	std::vector<glm::vec3> mColorInfo;
	std::vector<glm::vec3> mColorIDs;
	std::map<unsigned int, int> mColorToTileMap;

	int mNumPlayers;
	size_t mNumTiles;
	std::unique_ptr<Board> mGameBoard;

	bool mDoPick;
	enum KeyPress { NO_KEY_PRESS, LEFT_KEY, RIGHT_KEY, UP_KEY, DOWN_KEY };
	enum MouseClick { NO_MOUSE_CLICK, LEFT_MOUSE, MIDDLE_MOUSE, RIGHT_MOUSE };
	KeyPress mLastKeyPress;
	MouseClick mLastMouseClick;
	AxialCoord mSelectedTilePos;
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
		FancyCastlesView gameInstance(2);
		gameInstance.RenderLoop(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

