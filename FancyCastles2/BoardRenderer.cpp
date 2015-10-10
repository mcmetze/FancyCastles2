#define GLEW_STATIC
#include <GL/glew.h>
#include "SOIL.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "BoardRenderer.h"


namespace
{

	void
		GetCartesianFromAxial(GLfloat& x, GLfloat& y, const AxialCoord& position)
	{
		//gives a little space between tiles
		const float boarder = 0.01f;
		const int hexSize = 1;
		const float hexHeight = 2.0f * hexSize + boarder;
		const float hexWidth = 0.5f * sqrt(3.0f) * hexHeight + boarder;

		x = hexWidth * position.r + hexWidth / 2.f * position.q;
		y = -0.75f*hexHeight * position.q;
	}

	unsigned int
		GetUniqueTileColor(GLfloat& r, GLfloat& g, GLfloat& b, const int& index)
	{
		//convert the tile's index within the board to an RGB color
		unsigned char redVal = (index & 0x000000FF);
		unsigned char blueVal = (index & 0x0000FF00);
		unsigned char greenVal = (index & 0x00FF0000);

		r = redVal / 255.f;
		g = (greenVal >> 8) / 255.f;
		b = (blueVal >> 16) / 255.f;

		return redVal | (greenVal << 8) | (blueVal << 16);
	}

	std::string
		LoadShaderFile(const char* filename)
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

}

BoardRenderer::BoardRenderer(GLFWwindow* window, const float& width, const float& height) :
	  mWindow(window)
	, WINDOW_WIDTH(width)
	, WINDOW_HEIGHT(height)
	, mVertexShader(0)
	, mFragmentShader(0)
	, mShaderProgram(0)
	, mColorBufferPtr(nullptr)
{
}

void 
BoardRenderer::SetupTileColors(const std::vector<Color>& tileColors)
{
	GLfloat r, g, b;
	unsigned int tileIndex = 0;
	for (auto color : tileColors)
	{
		mColorInfo.push_back(glm::vec3(color.r, color.g, color.b));

		//used for picking later
		const auto rgbVal = GetUniqueTileColor(r, g, b, tileIndex);
		mColorToTileMap[rgbVal] = tileIndex;
		mColorIDs.push_back(glm::vec3(r, g, b));
		tileIndex++;
	}
}

void
BoardRenderer::SetupHexVerts(const std::vector<AxialCoord>& tileCoords)
{
	for (auto curTilePos : tileCoords)
	{
		//get a tile from the board and convert the axial coordinate to cartesian
		GLfloat x, y;
		GetCartesianFromAxial(x, y, curTilePos);

		mVertexInfo.push_back(glm::vec2(x, y));
	}
}

void
BoardRenderer::SetupTexture(const std::string& filename)
{
	BindTexture(GL_TEXTURE0, LoadTexture(filename));
	glUniform1i(glGetUniformLocation(mShaderProgram, "tileOutlineTex"), 0);
}

int 
BoardRenderer::GetTileFromPick(const unsigned int& rgbVal) const
{
	const auto foundTileIter = mColorToTileMap.find(rgbVal);
	if (foundTileIter != mColorToTileMap.end())
		return foundTileIter->second;

	return INT_MAX;
}

int  
BoardRenderer::DoPick()
{
	double mouseX = 0.0;
	double mouseY = 0.0;

	glfwGetCursorPos(mWindow, &mouseX, &mouseY);
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

	unsigned int result = data[0] | (data[1] << 8) | (data[2] << 16);
	
	return GetTileFromPick(result);
}

GLuint 
BoardRenderer::LoadTexture(std::string imagePath) const
{
	GLuint texture = SOIL_load_OGL_texture(imagePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

void 
BoardRenderer::BindTexture(GLenum TextureUnit, GLuint tex) const
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

bool 
BoardRenderer::CheckShader(const GLuint& shader) const
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

void 
BoardRenderer::SetupShaders()
{
	const auto vertexSource = LoadShaderFile("vert.glsl");
	const auto geometrySource = LoadShaderFile("geo.glsl");
	const auto fragmentSource = LoadShaderFile("frag.glsl");
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
	if (!success)
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

void 
BoardRenderer::SetupBuffers()
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

void 
BoardRenderer::SetupAttributes()
{
	// Specify the layout of the vertex data
	mPosAttrib = glGetAttribLocation(mShaderProgram, "position");
	glEnableVertexAttribArray(mPosAttrib);
	glVertexAttribPointer(mPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLint colAttrib = glGetAttribLocation(mShaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	mSelectedPointUniform = glGetUniformLocation(mShaderProgram, "selectedPoint");
}

void 
BoardRenderer::RenderPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*(mColorBufferPtr->size()), mColorBufferPtr->data(), GL_DYNAMIC_DRAW);

	glDrawArrays(GL_POINTS, 0, mVertexInfo.size());
}

void 
BoardRenderer::Cleanup()
{
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mGeometryShader);
	glDeleteShader(mFragmentShader);
	glDeleteShader(mVertexShader);

	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mColorBuffer);
}

void
BoardRenderer::SetSelection(const AxialCoord& position)
{
	GLfloat selectedPos[] = { 0.f, 0.f };

	//update the selected tile/outline texture
	GetCartesianFromAxial(selectedPos[0], selectedPos[1], position);
	glUniform2fv(mSelectedPointUniform, 1, selectedPos);
}

void 
BoardRenderer::RenderScene()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glVertexAttribPointer(mPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//second render pass with the real colors
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	mColorBufferPtr = &mColorInfo;

	RenderPass();

	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}



