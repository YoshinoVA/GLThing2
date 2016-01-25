#pragma once

#include <aie/Gizmos.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

class Camera
{
private:
	mat4 worldTransform;
	mat4 viewTransform;
	mat4 projectionTransform;

public:
	virtual void update(float deltaTime) = 0;	// abstract, to be implemented by classes that inherit from this
	void setPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	void setLookAt(vec3 From, vec3 To, vec3 Up);
	void setPosition(vec3 Position);

	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjection();
	mat4 getProjectionView();
};

class FlyCamera : public Camera
{
private:
	float speed;
	vec3 up;
public:
	virtual void update(float deltaTime) override;
	void setSpeed(float speed);
};

