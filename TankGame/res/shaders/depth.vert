#version 440 core
layout (location = 0) in vec3 position;

uniform mat4 DepthBiasMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = DepthBiasMatrix * model * vec4(position, 1.0);
}  