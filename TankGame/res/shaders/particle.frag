#version 450 core

layout (location = 0) out vec4 color;

in ParticleData
{
    vec2 textureCoord;
} particle;

layout (binding = 0 ) uniform sampler2D texture;

void main() {
    color = vec4(1,0,0,1);
}  