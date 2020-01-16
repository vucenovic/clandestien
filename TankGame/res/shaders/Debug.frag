#version 450 core

layout (location = 0) out vec3 color;

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
};

uniform int mode;

void main()
{
    switch(mode){
        case 0:
        color = TBN[0]; //worldspace Tangent
        break;
        case 1:
        color = TBN[1]; //worldspace Bitangent
        break;
        case 2:
        color = TBN[2]; //worldspace Normal
        break;
        case 3:
        color = TBN * vec3(0,0,1); //tangentspace to worldspace Normal
        break;
        case 4:
        color = worldPos; //Worldpos
        break;
        case 5:
        color = vec3(texCoord,1); //Texture coordinate
        break;
        case 6:
        color = normalize(eyeDir); //Eye direction
        break;
        color = color * 0.5 + 0.5;
    }
}