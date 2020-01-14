#version 450 core

layout (location = 0) out vec3 color;

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
};

void main()
{
	color = transpose(TBN) * vec3(0,0,1); //worldspace Normal
	//color = TBN[0]; //objectspace Normal
}