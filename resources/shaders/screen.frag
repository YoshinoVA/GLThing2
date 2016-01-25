#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D target;

vec4 Simple()
{
	return texture(target, vTexCoord);
}

void main()
{
	FragColor = Simple();
}