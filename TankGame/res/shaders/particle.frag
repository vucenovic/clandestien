#version 450 core

layout (location = 0) out vec4 color;

in ParticleData
{
    vec2 textureCoord;
};

layout (binding = 0 ) uniform sampler2D tex;

void main() {
    color = texture(tex, textureCoord);
    color.rgb = color.rgb * color.w;
}