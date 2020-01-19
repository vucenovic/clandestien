#version 450 core

layout (location = 0) in vec2 position;

out vec2 screenposition;

//Definitions
void main()
{
	gl_Position = vec4(position,0,0);
    screenposition = position;
}