#version 410

in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 FragColor;

uniform vec3 LightDir;

uniform sampler2D shadowMap;
uniform float shadowBias;

void main()
{ 
	float d = max(0, dot(normalize(vNormal.xyz), LightDir)); 
	
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias)
	{
		d = 0.5;
	}
	
	FragColor = vec4(d,d,d,1);
}