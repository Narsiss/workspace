#version 130

flat in vec4 vFlatColor;

out vec4 vFragColor;

void main(void)
{
	vFragColor = vFlatColor;
}