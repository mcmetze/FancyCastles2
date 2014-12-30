#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
//#include <glm.hpp>

#include <vld.h>

#include "Board.h"
#include "HexGeometry.h"

static const float WINDOW_WIDTH = 1366;
static const float WINDOW_HEIGHT = 768;
static const float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

// Shader sources
const GLchar* vertexSource =
"#version 330 \n"
"in vec2 position;"
"in vec3 color;"
"out vec3 vColor;"
"void main() {"
"   vColor = color;"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";

const GLchar* geometrySource =
"#version 330\n"
"#define PI 3.14159265358979323846 \n"
" layout(points) in;"
" layout(triangle_strip, max_vertices = 12) out;"
" in vec3 vColor[];"
" uniform vec2 selectedPoint;"
" out vec3 fColor;"
" out vec2 fTexcoord;"
" void main() "
" {"
"       vec2 texc[6] = vec2[](vec2(0.0, 0.0), "
"                             vec2(0.0, 0.0), "
"                             vec2(0.0, 0.0), "
"                             vec2(0.0, 0.0), "
"                             vec2(0.0, 0.0), "
"                             vec2(0.0, 0.0)); "
"		float w = 1366.f/100.f;"
"		float h = 768.f/100.f;"
"		vec2 verts[6]; "
"		for (int corner = 0; corner < 6; corner++)"
"		{ "
"			float angle = 2.0 * PI / 6.0 * (corner + 0.5); "
"			float cornerX = gl_in[0].gl_Position.x + cos(angle);	"
"			float cornerY = gl_in[0].gl_Position.y + sin(angle); "
"			verts[corner] = vec2(cornerX/w, cornerY/h); "
"		} "
"       if (selectedPoint.x == gl_in[0].gl_Position.x "
"         && selectedPoint.y == gl_in[0].gl_Position.y )"
"       { "
"           texc = vec2[](vec2(1.0, 0.75), "
"                  vec2(0.5, 1.0), "
"                  vec2(0.0, 0.75), "
"                  vec2(0.0, 0.25), "
"                  vec2(0.5, 0.0), "
"                  vec2(1.0, 0.25)); "
"       }"
"		fColor = vColor[0];"
"       int curVert = 0;"
"       for(int triangle = 0; triangle < 4; triangle++) "
"       {"
"			gl_Position =  vec4(verts[0].xy, 0.0, 1.0); "
"           fTexcoord = texc[0]; "
"			EmitVertex(); "
"			gl_Position =  vec4(verts[curVert+1].xy, 0.0, 1.0); "
"           fTexcoord = texc[curVert+1]; "
"			EmitVertex(); "
"			gl_Position =  vec4(verts[curVert+2].xy, 0.0, 1.0); "
"           fTexcoord = texc[curVert+2]; "
"			EmitVertex(); "
"           curVert++; "
"		} "
"		EndPrimitive(); "
"}";

const GLchar* fragmentSource =
"#version 330 \n"
"in vec3 fColor;"
"in vec2 fTexcoord;"
"out vec4 fragColor;"
"uniform sampler2D tileOutlineTex;"
"void main() {"
"   vec4 outlineColor = texture(tileOutlineTex, fTexcoord);"
"   if(outlineColor.a == 0)"
"   { "
"       outlineColor = vec4(fColor, 1.0);"
"   }"
"   fragColor = outlineColor; "
"}";


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

	void SetupTileColors()
	{
		GLfloat r, g, b;
		for (auto tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//TODO: use textures
			auto tileType = mGameBoard->GetTileType(tileIndex);
			GetVertexColorFromType(r, g, b, tileType);
			mColorInfo.push_back(r);
			mColorInfo.push_back(g);
			mColorInfo.push_back(b);

			//used for picking later
			GetUniqueTileColor(r, g, b, tileIndex);
			mColorIDs.push_back(r);
			mColorIDs.push_back(g);
			mColorIDs.push_back(b);
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
		for (auto tileIndex = 0; tileIndex < mNumTiles; ++tileIndex)
		{
			//get a tile from the board and convert the axial coordinate to cartesian
			GLfloat x, y; 
			GetCartesianFromAxial(x, y, mGameBoard->GetTileCoord(tileIndex));

			mVertexInfo.push_back(x);
			mVertexInfo.push_back(y);
		}
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

	void SetupShaders()
	{
		// Create and compile the vertex shader
		mVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(mVertexShader, 1, &vertexSource, NULL);
		glCompileShader(mVertexShader);
		CheckShader(mVertexShader);

		GLuint mGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(mGeometryShader, 1, &geometrySource, NULL);
		glCompileShader(mGeometryShader);
		CheckShader(mGeometryShader);

		// Create and compile the fragment shader
		mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(mFragmentShader, 1, &fragmentSource, NULL);
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
		mColorBufferPtr = &mColorInfo;

		SetupShaders();
		BindTexture(GL_TEXTURE0, LoadTexture("TileOutline.png"));
		glUniform1i(glGetUniformLocation(mShaderProgram, "tileOutlineTex"), 0);
		GLint selectedPointUniform = glGetUniformLocation(mShaderProgram, "selectedPoint");
		mSelectedTilePos = AxialCoord(mNumTiles, mNumTiles);

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(mShaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		GLint colAttrib = glGetAttribLocation(mShaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		bool colorsBufferDirty = false;
		bool leftMousePressed = false;
		bool leftKeyPressed = false;
		bool rightKeyPressed = false;
		bool upKeyPressed = false;
		bool downKeyPressed = false;

		double mouseX = 0.0;
		double mouseY = 0.0;
		while (!glfwWindowShouldClose(window))
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				leftKeyPressed = true;
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE && leftKeyPressed)
			{
				leftKeyPressed = false;
				mSelectedTilePos.r--;
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				rightKeyPressed = true;
			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE && rightKeyPressed)
			{
				rightKeyPressed = false;
				mSelectedTilePos.r++;
			}

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				upKeyPressed = true;
			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && upKeyPressed)
			{ 
				upKeyPressed = false;
				mSelectedTilePos.q--;
			}

			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				downKeyPressed = true;
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && downKeyPressed)
			{
				downKeyPressed = false;
				mSelectedTilePos.q++;
			}

			// flag the click for release
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				leftMousePressed = true;
			}

			// released from a left mouse click, do the first render pass for the pick operation
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && leftMousePressed)
			{
				leftMousePressed = false;
				colorsBufferDirty = true;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				mouseY = WINDOW_HEIGHT - mouseY;

				//clear to white because black is generated in the unique tile colors for index 0
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//use the color buffer with the unique tile colors
				mColorBufferPtr = &mColorIDs;
				glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);

				glDrawArrays(GL_POINTS, 0, mVertexInfo.size()/2);

				glFlush();
				glFinish();

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				//read the pixel at the mouse position and look up the tile that maps to this color in the color/tile map
				unsigned char data[4];
				glReadPixels(static_cast<GLint>(mouseX), static_cast<GLint>(mouseY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
				unsigned int rgbVal = data[0] | (data[1] << 8) | (data[2] << 16);

				if (mColorToTileMap.find(rgbVal) == mColorToTileMap.end())
				{
					printf("DID NOT FIND TILE\n");
					printf("%#04x, %#04x, %#04x\n", data[0], data[1], data[2]);
					mSelectedTilePos = AxialCoord(mNumTiles, mNumTiles);
				}
				else
				{
					int tileID = mColorToTileMap[rgbVal];
					printf("found tile %i\n", tileID);
					PrintTileInfo(tileID);
					mSelectedTilePos = mGameBoard->GetTileCoord(tileID);
				}
			}

			//second render pass with the real colors
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

			if (colorsBufferDirty)
			{
				colorsBufferDirty = false;
				mColorBufferPtr = &mColorInfo;
				glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);
			}

			GLfloat selectedX, selectedY;
			GetCartesianFromAxial(selectedX, selectedY, mSelectedTilePos);
			GLfloat selectedPos[] = { selectedX, selectedY };
			glUniform2fv(selectedPointUniform, 1, selectedPos);

			glDrawArrays(GL_POINTS, 0, mVertexInfo.size()/2);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteProgram(mShaderProgram);
		glDeleteShader(mGeometryShader);
		glDeleteShader(mFragmentShader);
		glDeleteShader(mVertexShader);

		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &colorBuffer);
	}

private:

	GLuint mVertexShader;
	GLuint mGeometryShader;
	GLuint mFragmentShader;
	GLuint mShaderProgram;

	int mNumPlayers;
	size_t mNumTiles;
	std::unique_ptr<Board> mGameBoard;
	AxialCoord mSelectedTilePos;

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
		FancyCastlesView gameInstance(4);
		gameInstance.RenderLoop(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

