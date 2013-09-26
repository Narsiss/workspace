#version 330
out vec4 vFragColor;

uniform samplerCube cubeMap;

varying vec3 vVaryingTexCoord;

void main()
{
	vFragColor = textureCube(cubeMap,vVaryingTexCoord);
}

    