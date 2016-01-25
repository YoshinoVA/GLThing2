#pragma once

#include <gl_core_4_4.h>
#include "GLFW\glfw3.h"
#include "Camera.h"
#include "fbx\FBXFile.h"

struct RenderObject
{
	uint VBO, IBO, VAO, indexCount;
};

class Application
{
public:
	GLFWwindow * window;

	virtual bool init() { return true; };	// start the app
	virtual bool update() { return true; };	// update and perform calculations
	virtual void draw() {};	// renders the application
	virtual void exit() {};	// exits and performs cleanup
};

class DemoApp : public Application
{
	virtual bool init();
	virtual bool update();
	virtual void draw();
			void oldDraw(); // for reference only
	virtual void exit();
	FlyCamera camera;

	unsigned int modelShader;
	unsigned int screenShader;
	unsigned int shadowShader;

	unsigned int indexCount;
	unsigned int VBO, IBO, VAO, FBO;

	// VBO - Vertex Buffer Object
	//  - storing data/information pertaining to the vertices for a mesh
	//  - position, texcoord
	// IBO - Index Buffer Object
	//  - storing the order in which these vertices are drawn
	// VAO - Vertex Array Object
	//  - a container for the VBO and IBO
	// FBO - Framebuffer Object
	//  - a buffer that meshes can be drawn to

	unsigned int texture;
	unsigned int normalmap;
	unsigned int fboTexture, fboDepth;

	float currentTime = 0;
	float lastTime = 0;
	float deltaTime = 0;

	//void generateGrid(unsigned int rows, unsigned int cols);
	RenderObject generateQuad();

	RenderObject quad;

	FBXFile* fbx;

	unsigned int program;
	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
};