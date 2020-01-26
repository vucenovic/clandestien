#version 450 core

layout (location = 0) out vec4 outColor;

in ParticleData
{
    vec4 color;
    vec2 textureCoord;
};

layout (binding = 0 ) uniform sampler2D tex;

void main() {
    outColor = texture(tex, textureCoord);
}