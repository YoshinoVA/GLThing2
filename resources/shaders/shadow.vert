#version 410

layout (location = 0) in vec4 position;

uniform mat4 lightMatrix;

void main()
{	
	gl_Position = lightMatrix * position;
}