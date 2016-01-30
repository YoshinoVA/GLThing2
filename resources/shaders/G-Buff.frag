#version 410

in vec4 vPosition;
in vec4 vNormal;

layout(location = 0) out vec3 gpassAlbedo;
layout(location = 1) out vec3 gpassPosition;
layout(location = 2) out vec3 gpassNormal;

void main() {
	gpassAlbedo  = vec3(1);
	gpassPosition = vPosition.xyz;
	gpassNormal   = vNormal.xyz;
}