#version 450 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D texbloom;

in vec2 screenposition;

uniform float gamma = 1;

uniform bool combine;
uniform float cutoff = 2;
uniform float exposure = 1;

//yes having multiple shaders would make them a little bit faster but we are not worried about the performance of this little operation
void main() {
    if(!combine){ //filter 
        vec4 c = texture(tex, screenposition);
        float brightness = dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
        color = brightness > cutoff ? c : vec4(0,0,0,1);
    }
    else{//combine and do tone mapping
        vec3 c = texture(tex, screenposition).xyz + texture(texbloom, screenposition).xyz;
        c = vec3(1) - exp(-c * exposure);
        color = vec4(pow(c, vec3(1/gamma)),1);
    }
}
