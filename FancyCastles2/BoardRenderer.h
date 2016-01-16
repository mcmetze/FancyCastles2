#pragma once

#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <vector>
#include <unordered_map>

#include "TileTraits.h"

class Board;

class BoardRenderer
{

public:
	BoardRenderer(GLFWwindow*, int, int);
	void Init();

	void SetupTiles(const Board& gameBoard);
	void SetSelection(const AxialCoord& position);

	int DoPick();
	void RenderScene();

	void Cleanup();

private:
	void SetupHexVerts(const std::vector<AxialCoord>& tileCoords);
	void SetupTileColors(const std::vector<Color>& tileColors);
	void SetupBuffers();
	void SetupShaders();
	void SetupAttributes();
	void SetupTexture(const std::string&);

	int GetTileFromPick(const unsigned int& rgbVal) const;

	GLuint LoadTexture(std::string imagePath) const;
	void BindTexture(GLenum TextureUnit, GLuint tex) const;

	bool CheckShader(const GLuint& shader) const;

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
	std::unordered_map<unsigned int, int> mColorToTileMap;
};
