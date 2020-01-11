#version 450 core

layout (location = 0) out vec3 color;

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec3 worldNormal;
	vec2 texCoord;
};

void main()
{
	color = normalize(worldNormal);
}