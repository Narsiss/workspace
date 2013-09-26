#version 330
uniform samplerCube cubeMap;

varying vec3 vVaryingTexCoords;

out vec4 vFragColor;

void main()
{
	vFragColor = textureCube(cubeMap,vVaryingTexCoords);
}
