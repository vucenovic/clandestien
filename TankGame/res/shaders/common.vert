#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 tangent;

layout (binding = 0, std140) uniform viewData
{
	mat4 viewProjection;
	vec4 eyePos;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

out VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
} vertex;

//Definitions
void main()
{
	vec4 worldPos = modelMatrix*vec4(position,1);

	vec3 worldNormal = (modelNormalMatrix*vec4(normal,0)).xyz;
	//vertex.worldNormal = (modelNormalMatrix*vec4(normal,0)).xyz;
	
	//vec3 tangent = cross(worldNormal,tangent.xyz);
	vertex.TBN = mat3(tangent.xyz,cross(tangent.xyz,worldNormal) * tangent.w ,normalize(worldNormal));

	vertex.eyeDir = (eyePos - worldPos).xyz;
	vertex.worldPos = worldPos.xyz;

	gl_Position = viewProjection*worldPos;
	vertex.texCoord = texCoord;
}