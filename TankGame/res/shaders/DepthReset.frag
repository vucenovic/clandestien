#version 450 core

layout (location = 0) out vec4 color;

in vec2 screenposition;

void main() {
    gl_FragDepth = 1;
    color = vec4(0);
}
