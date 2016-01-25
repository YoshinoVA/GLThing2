#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 Normal;

out vec4 vNormal;
out vec4 vShadowCoord;

uniform mat4 ProjectionView;
uniform mat4 lightMatrix;

void main()
{
	vNormal = Normal;
	gl_Position = lightMatrix * position;
	
	vShadowCoord = lightMatrix * position;
}