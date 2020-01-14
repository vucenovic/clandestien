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
	color = vec3(texCoord,1);
}