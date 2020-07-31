#version 450 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex;

in vec2 screenposition;

void main() {
    color = texture(tex, screenposition);
    color.rgb = color.rgb / color.w;
}