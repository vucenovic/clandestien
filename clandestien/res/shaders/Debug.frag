#version 450 core

layout (location = 0) out vec4 color;

layout (binding = 0, std140) uniform viewData
{
	mat4 projection;
	mat4 view;
};

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
    vec4 shadowPos;
};

uniform int mode;

void main()
{
    vec3 tempColor = vec3(0);
    switch(mode){
        case 0:
        tempColor = TBN[0]; //worldspace Tangent
        break;
        case 1:
        tempColor = TBN[1]; //worldspace Bitangent
        break;
        case 2:
        tempColor = TBN[2]; //worldspace Normal
        break;
        case 3:
        tempColor = TBN * vec3(0,0,1); //tangentspace to worldspace Normal
        break;
        case 4:
        tempColor = worldPos; //Worldpos
        break;
        case 5:
        tempColor = vec3(texCoord,1); //Texture coordinate
        break;
        case 6:
        tempColor = normalize(eyeDir); //Eye direction
        break;
    }
    color = vec4(tempColor,1);
}