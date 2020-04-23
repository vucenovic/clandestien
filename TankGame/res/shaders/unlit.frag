#version 450 core

layout (location = 0) out vec4 color;

uniform vec3 flatColor;

void main()
{
	color = vec4(flatColor,1);
}
