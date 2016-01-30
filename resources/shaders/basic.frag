#version 410

in vec2 vTexCoord;
in vec3 Normal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec4 vShadowCoord;

out vec4 FragColor;
out vec4 vNormal;

uniform vec3 LightDir;

uniform sampler2D normal; 
uniform sampler2D diffuse;
uniform sampler2D shadowMap;
uniform float shadowBias;

void main()
{ 	
	mat3 TBN = mat3(normalize(vTangent), normalize(vBiTangent), normalize(vNormal));
	vec3 N = texture(normal, vTexCoord).xyz * 2 -1;
	
	float d = max(0, dot(normalize(TBN * N),normalize(LightDir))); 
	
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias)
	{
		d = 0.5;
	}
	
	FragColor = texture(diffuse, vTexCoord);
}