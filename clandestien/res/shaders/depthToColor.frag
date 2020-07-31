#version 450 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex;

in vec2 screenposition;

uniform float scaleFactor = 25;

void main() {
    float depth = texture(tex,screenposition).x;
    float depth2 = pow(depth,scaleFactor);
    color = vec4(vec3(depth2),1);
}
