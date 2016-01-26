#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Normal; 

out vec4 vNormal;
out vec4 vShadowCoord;

uniform mat4 Model;
uniform mat4 ProjectionView; 

uniform mat4 lightMatrix;

void main()
{ 
	vNormal = Normal; 
	gl_Position= ProjectionView * Model * Position;
	
	vShadowCoord = lightMatrix * Position;
}