#ifndef METZE_BOARD_RENDERER_H
#define METZE_BOARD_RENDERER_H
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <glm.hpp>

#include "Board.h"

class BoardRenderer
{

public:

	BoardRenderer(GLFWwindow*, const float&, const float&);

	void SetupHexVerts(const std::vector<AxialCoord>& tileCoords);
	void SetupTileColors(const std::vector<Color>& tileColors);
	void SetupBuffers();
	void SetupShaders();
	void SetupAttributes();
	void SetupTexture(const std::string&);

	void SetSelection(const AxialCoord& position);
	int DoPick();
	void RenderScene();
	void Cleanup();

private:

	void GetCartesianFromAxial(GLfloat& x, GLfloat& y, const AxialCoord& position);
	void GetUniqueTileColor(GLfloat& r, GLfloat& g, GLfloat& b, const int& index);

	int GetTileFromPick(const unsigned int& rgbVal);

	GLuint LoadTexture(std::string imagePath);
	void BindTexture(GLenum TextureUnit, GLuint tex);

	bool CheckShader(const GLuint& shader);
	std::string LoadShaderFile(const char* filename);

	void RenderPass();

	GLFWwindow* mWindow;
	const float WINDOW_WIDTH;
	const float WINDOW_HEIGHT;

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
};

#endif