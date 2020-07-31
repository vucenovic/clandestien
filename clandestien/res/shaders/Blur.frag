#version 450 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex;

in vec2 screenposition;

uniform bool horizontal;
uniform float scale = 1.0;
uniform int kernelSize = 5;
uniform float weights[10] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216,0,0,0,0,0); //preset weights

void main() {
    vec2 pixelSize = scale / textureSize(tex,0);
    vec3 c = texture(tex,screenposition).rgb * weights[0];

    if(horizontal){
        for(int i = 1;i<kernelSize;i++){
            vec2 offset = vec2(pixelSize.x * i, 0);
            c += texture(tex,screenposition + offset).rgb * weights[i];
            c += texture(tex,screenposition - offset).rgb * weights[i];
        }
    }
    else{
        for(int i = 1;i<kernelSize;i++){
            vec2 offset = vec2(0, pixelSize.y * i);
            c += texture(tex,screenposition + offset).rgb * weights[i];
            c += texture(tex,screenposition - offset).rgb * weights[i];
        }
    }
    color = vec4(c,1);
}