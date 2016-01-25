#version 410

in vec2 vTexCoord; 
in vec4 vNormal; 

out vec4 FragColor;

uniform sampler2D diffuse; 
uniform vec3 LightDir; 

void main()
{ 
	float d = max(0, dot(normalize(vNormal.xyz), LightDir)); 
	FragColor = texture(diffuse, vTexCoord);
}