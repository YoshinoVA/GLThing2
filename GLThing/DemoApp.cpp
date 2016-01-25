#include "Application.h"

#include <aie\Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <iostream>
#include <fstream>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#include "fbx\FBXFile.h"

using namespace glm;

struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
};

// Shadow Process
// 1. Get an empty screen
// 2. Get a quad on the "ground"
// 3. Get any sort of model above the ground
// 4. Shadows

int loadShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
	// load vertex shader
	std::ifstream inVert(vertexShaderPath);
	std::string  vertexShaderContents((std::istreambuf_iterator<char>(inVert)), std::istreambuf_iterator<char>());

	char * vertexSource = new char[vertexShaderContents.length() + 1]; // '\0' - null terminating character
	strncpy_s(vertexSource, vertexShaderContents.length() + 1, vertexShaderContents.c_str(), vertexShaderContents.length());

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vertexSource, 0);
	glCompileShader(vertexShader);

	// load fragment shader
	std::ifstream inFrag(fragmentShaderPath);
	std::string  fragmentShaderContents((std::istreambuf_iterator<char>(inFrag)), std::istreambuf_iterator<char>());

	char * fragmentSource = new char[fragmentShaderContents.length() + 1]; // '\0' - null terminating character
	strncpy_s(fragmentSource, fragmentShaderContents.length() + 1, fragmentShaderContents.c_str(), fragmentShaderContents.length());

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fragmentSource, 0);
	glCompileShader(fragmentShader);

	// assembling the shader program
	int shaderName = glCreateProgram();
	glAttachShader(shaderName, vertexShader);
	glAttachShader(shaderName, fragmentShader);
	glLinkProgram(shaderName);

	// check that the shader compiled successfully without error
	int success = GL_FALSE;

	glGetProgramiv(shaderName, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shaderName, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(shaderName, GL_INFO_LOG_LENGTH, 0, infoLog);
		printf("Error: Failed to link shader program.\n");
		printf("%s\n", infoLog);
		delete infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderName;
}

void DemoApp::createOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glEnableVertexAttribArray(2); // normal
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(3); // tangent
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);

		glBindVertexArray(0);	// unbind VAO (this has to come first)
		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind IBO

		mesh->m_userData = glData;
	}
}

void DemoApp::cleanupOpenGLBuffers(FBXFile* fbx)
{
	if (fbx == nullptr)
		return;

	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

//void DemoApp::generateGrid(unsigned int rows, unsigned int cols)
//{
//	Vertex* aoVerts = new Vertex[rows * cols];
//	for (unsigned int r = 0; r < rows; ++r)
//	{
//		for (unsigned int c = 0; c < cols; ++c)
//		{
//			aoVerts[r * cols + c].position = vec4(
//				(float)c, 0, (float)r, 1);
//
//			vec3 color = vec3(sinf((c / (float)(cols - 1))*(r / (float)(rows - 1))));
//			aoVerts[r*cols + c].color = vec4(color, 1);
//		}
//	}
//
//	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
//
//	unsigned int index = 0;
//	for (unsigned int r = 0; r < (rows - 1); ++r)
//	{
//		for (unsigned int c = 0; c < (cols - 1); ++c)
//		{
//			//Tri 1
//			auiIndices[index++] = r * cols + c;
//			auiIndices[index++] = (r + 1) * cols + c;
//			auiIndices[index++] = (r + 1) * cols + (c + 1);
//
//			//Tri 2
//			auiIndices[index++] = r * cols + c;
//			auiIndices[index++] = (r + 1) * cols + (c + 1);
//			auiIndices[index++] = r * cols + (c + 1);
//		}
//	}
//
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &IBO);
//	glGenVertexArrays(1, &VAO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex),
//		aoVerts, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//		(void*)(sizeof(vec4)));
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	indexCount = (rows - 1) * (cols - 1) * 6;
//
//	delete[] aoVerts;
//	delete[] auiIndices;
//}

RenderObject DemoApp::generateQuad()
{
	uint vbo, ibo, vao, localIndexCount;

	float vertexData[] = {
	// Position		  // ST (or UVs)
	//   X, Y, Z, W     S, T
		-5,  0, 5, 1,	0, 1, // Vertex 1
		 5,  0, 5, 1,	1, 1, // Vertex 2
		 5,  0, -5, 1,	1, 0,
		-5,  0, -5, 1,	0, 0,
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);		// Generation
	glBindBuffer(GL_ARRAY_BUFFER, vbo);		// Bind it, setting it as the current whatever
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	// Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);	// TexCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) +16);

	// once we're done, unbind the buffers
	glBindVertexArray(0);						// VAO comes off first
	glBindBuffer(GL_ARRAY_BUFFER, 0);			// VBO or IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// whatever's left

	localIndexCount = 6;

	return {vbo, ibo, vao, localIndexCount};
}

// - creates the window
// - loads the shaders
// - loads the external assets
//   - textures
//   - models/meshes
bool DemoApp::init()
{
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	// START RENDERING CODE ------------------

	modelShader  = loadShader("../resources/shaders/basic.vert","../resources/shaders/basic.frag");
	screenShader = loadShader("../resources/shaders/screen.vert", "../resources/shaders/screen.frag");
	shadowShader = loadShader("../resources/shaders/shadow.vert", "../resources/shaders/shadow.frag");

	// loading textures -------------------------
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	int imageWidth = 0;
	int imageHeight = 0;
	int imageFormat = 0;

	//Load Diffuse mapping
	unsigned char* data = stbi_load("../resources/FBX/soulspear/soulspear_diffuse.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	//Load Normal Mapping
	data = stbi_load("../resources/FBX/soulspear/soulspear_normal.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &normalmap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// generate FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// generate and bind a texture for the FBO
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTexture, 0);

	// generate and bind a depth texture for the FBO
	glGenRenderbuffers(1, &fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);

	//16-bit depth component for FBO texture
	glGenTextures(1, &fboDepth);
	glBindTexture(GL_TEXTURE_2D, fboDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH, GL_DEPTH_COMPONENT16, 800, 600, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fboDepth, 0);

	// assign attachments to the FBO
	//  - attachments tell the FBO what textures to render the buffer onto
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	//glDrawBuffer(GL_NONE);

	// validate/verify that the FBO is in working order
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error! U dun fucked up.\n");

	// unbind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Load meshes ---------------------
	fbx = new FBXFile();
	fbx->load("../resources/FBX/soulspear/soulspear.fbx");
	//fbx->load("../resources/textures/crate.png");
	createOpenGLBuffers(fbx);

	//generateGrid(10, 10);
	quad = generateQuad();

	Gizmos::create();

	return true;
}

// - performing calculations every frame
// - user input
bool DemoApp::update()
{
	
	//glEnable(GL_DEPTH_TEST);

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	// if the user presses escape, exit from the program
	if (glfwWindowShouldClose(window) == true ||
		glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		return false;
	}

	// delta time
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	return true;
}

// - drawing meshes to the backbuffer
void DemoApp::draw()
{
	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(projection * view);

	glUseProgram(modelShader);
	int loc = glGetUniformLocation(modelShader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalmap);

	loc = glGetUniformLocation(modelShader, "diffuse");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(modelShader, "normal");
	glUniform1i(loc, 1);

	if (fbx != nullptr)
	{
		for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = fbx->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(quad.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void DemoApp::oldDraw()
{
	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	//set the direction of the light
	glm::vec3 lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));
	glm::mat4 lightProj = glm::ortho<float>(-10, 10, -10, 10, -10, 10);

	// DRAW TO FRAMEBUFFER-----------

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 800, 600);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	//Gizmos::draw(projection * view);

	//glUseProgram(modelShader);
	int loc = glGetUniformLocation(modelShader, "ProjectionView");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection * view));

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normalmap);

	//loc = glGetUniformLocation(modelShader, "diffuse");
	//glUniform1i(loc, 0);

	//loc = glGetUniformLocation(modelShader, "normal");
	//glUniform1i(loc, 1);

	glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };

	loc = glGetUniformLocation(modelShader, "LightColor");
	glUniform3fv(loc, 1, glm::value_ptr(lightColor));

	glm::vec3 lightDir = { 0.0f, 1.0f, 0.0f };

	loc = glGetUniformLocation(modelShader, "LightDir");
	glUniform3fv(loc, 1, glm::value_ptr(lightDir));

	glm::vec3 cameraPos = { 0.3f, 0.3f, 0.3f };

	loc = glGetUniformLocation(modelShader, "CameraPos");
	glUniform3fv(loc, 1, glm::value_ptr(cameraPos));

	glm::float1 specPow = (128.0f);

	loc = glGetUniformLocation(modelShader, "SpecPow");
	glUniform1f(loc, 128.0f);

	if (fbx != nullptr)
	{
		for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = fbx->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	// DRAW TO SCREEEEEEEEEEN-----------

	// bind the framebuffer of the screen (0)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);

	//glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Gizmos::clear();
	//Gizmos::addTransform(glm::mat4(1));
	//Gizmos::draw(projection * view);

	glUseProgram(screenShader);

	loc = glGetUniformLocation(screenShader, "target");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glUniform1i(loc, 0);

	// draw a quad with that texture
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	//glBindVertexArray(quad.VAO);
	//glDrawElements(GL_TRIANGLES, quad.indexCount, GL_UNSIGNED_INT, nullptr);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

// - clean up
// - deleting opengl buffers that we've created
// - unloading any file data that we have open
// - destroy the window
void DemoApp::exit()
{
	cleanupOpenGLBuffers(fbx);
	glDeleteProgram(program);
	if (fbx != nullptr)
		fbx->unload();
	Gizmos::destroy();
	glfwTerminate();
}