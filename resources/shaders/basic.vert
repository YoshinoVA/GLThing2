#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec2 TexCoord; 
layout(location=2) in vec4 Normal; 

out vec2 vTexCoord; 
out vec4 vNormal; 

uniform mat4 ProjectionView; 

void main()
{ 
	vTexCoord = TexCoord; 
	vNormal = Normal; 
	gl_Position= ProjectionView * Position;
}