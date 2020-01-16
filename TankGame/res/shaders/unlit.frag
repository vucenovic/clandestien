#version 450 core

layout (location = 0) out vec3 color;

uniform vec3 flatColor;

void main()
{
	color = flatColor;
}