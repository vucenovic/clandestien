#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

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
	vec3 worldNormal;
	vec2 texCoord;
} vData;

//Definitions
void main()
{
	vec4 worldPos = modelMatrix*vec4(position,1);

	vData.worldNormal = (modelNormalMatrix*vec4(normal,0)).xyz;
	vData.eyeDir = (eyePos - worldPos).xyz;
	vData.worldPos = worldPos.xyz;

	gl_Position = viewProjection*worldPos;
	vData.texCoord = texCoord;
}
